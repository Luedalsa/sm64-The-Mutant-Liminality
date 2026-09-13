//
// Constraint detection and resolution for the semantic generation graph.
//

#include "Constraintsolver.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <limits>
#include <unordered_set>

RelationId ConstraintSolver::addRelation(Relation r) {
    const RelationId id = r.id;
    relations_[id] = std::move(r);
    return id;
}

int32_t ConstraintSolver::addEdge(Edge e) {
    if (e.geometricRelations < 0) e.geometricRelations = e.relation;
    if (e.relation < 0) e.relation = e.geometricRelations;

    int32_t id = e.id;
    if (id < 0) {
        id = edges_.empty() ? 0 : edges_.begin()->first + 1;
        e.id = id;
    }
    if (e.positionHint == 0.0f) e.positionHint = e.transform.distance;
    if (e.orientation == 0.0f) e.orientation = e.transform.yaw;
    if (e.size == 0.0f) e.size = e.transform.distance;

    edges_[id] = e;
    edgesByRelation_.emplace(e.geometricRelations, id);
    return id;
}

Relation& ConstraintSolver::relation(RelationId id) { return relations_.at(id); }
Edge& ConstraintSolver::edge(int32_t id) { return edges_.at(id); }

void ConstraintSolver::log(const std::string& tag, const std::string& msg) const {
    const char* color = "\033[33m";
    if (tag == "OK") color = "\033[32m";
    if (tag == "INFO") color = "\033[36m";
    std::cout << color << "[" << tag << "] " << msg << "\033[0m" << std::endl;
}

bool ConstraintSolver::checkSymmetry(int32_t edgeId, int32_t counterpartEdgeId) const {
    const auto counterpart = edges_.find(counterpartEdgeId);
    if (counterpart == edges_.end()) return false;
    const Edge& a = edges_.at(edgeId);
    const Edge& b = counterpart->second;
    return std::abs(std::abs(a.orientation) - std::abs(b.orientation)) < 1e-3f;
}

bool ConstraintSolver::checkProximityMerge(int32_t a, int32_t b, float maxDistance) const {
    const Edge& ea = edges_.at(a);
    const Edge& eb = edges_.at(b);
    return ea.geometricRelations == eb.geometricRelations &&
           std::abs(ea.positionHint - eb.positionHint) <= maxDistance;
}

bool ConstraintSolver::isToleranceExceeded(int32_t edgeId) const {
    const Edge& e = edges_.at(edgeId);
    const Relation& r = relations_.at(e.geometricRelations);
    if (r.toleranceMode == ToleranceMode::Explicit) {
        return std::abs(e.size - r.baseSize) > r.toleranceValue;
    }
    return e.size < r.derivedMin || e.size > r.derivedMax;
}

void ConstraintSolver::mergeEdges(int32_t survivor, int32_t absorbed) {
    Edge& a = edges_.at(absorbed);
    a.merged = true;
    a.mergedInto = survivor;

    auto range = edgesByRelation_.equal_range(a.geometricRelations);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == absorbed) {
            edgesByRelation_.erase(it);
            break;
        }
    }
    log("MERGE", "arista " + std::to_string(absorbed) +
                 " fusionada en arista " + std::to_string(survivor));
}

void ConstraintSolver::lockOrientationGrow(int32_t edgeId, float requiredDelta) {
    Edge& e = edges_.at(edgeId);
    if (!e.orientationLocked) {
        e.orientation += requiredDelta;
        return;
    }
    e.size += requiredDelta;
    log("LOCK", "arista " + std::to_string(edgeId) +
                " no puede modificar orientación; incrementa tamaño");
}

void ConstraintSolver::resolveDerivedTolerance(RelationId repeatedRelationId) {
    Relation& r = relations_.at(repeatedRelationId);
    if (!r.isRepeated || r.components.empty()) return;

    const RelationId unit = r.components[0];
    float minimum = std::numeric_limits<float>::max();
    float maximum = std::numeric_limits<float>::lowest();
    int count = 0;
    const auto range = edgesByRelation_.equal_range(unit);
    for (auto it = range.first; it != range.second; ++it) {
        const Edge& e = edges_.at(it->second);
        if (e.merged) continue;
        minimum = std::min(minimum, e.size);
        maximum = std::max(maximum, e.size);
        ++count;
    }
    if (count == 0) return;

    r.derivedMin = minimum;
    r.derivedMax = maximum;
    r.repeatCount = count;
    log("INFO", "relación " + std::to_string(repeatedRelationId) +
                " derivó tolerancia [" + std::to_string(minimum) + ", " +
                std::to_string(maximum) + "]");
}

std::vector<RelationId> ConstraintSolver::parentsOf(RelationId child) const {
    std::vector<RelationId> parents;
    for (const auto& entry : relations_) {
        const Relation& relation = entry.second;
        if (std::find(relation.components.begin(), relation.components.end(), child) !=
            relation.components.end()) {
            parents.push_back(entry.first);
        }
    }
    return parents;
}

int32_t ConstraintSolver::representativeEdgeFor(RelationId id) const {
    const auto it = edgesByRelation_.find(id);
    return it == edgesByRelation_.end() ? -1 : it->second;
}

void ConstraintSolver::propagateConstraint(RelationId id) {
    log("PROPAGATE", "relación " + std::to_string(id) +
                     " superó su tolerancia; propagando constraint");
    for (RelationId parent : parentsOf(id)) {
        Relation& relation = relations_.at(parent);
        if (relation.invariant == InvariantMode::SumPreserving) {
            enforceSumInvariant(parent);
        } else if (relation.isRepeated) {
            resolveDerivedTolerance(parent);
        }
    }
}

void ConstraintSolver::enforceSumInvariant(RelationId compositeId) {
    Relation& composite = relations_.at(compositeId);
    if (composite.invariant != InvariantMode::SumPreserving) return;

    float currentSum = 0.0f;
    std::vector<int32_t> representatives;
    for (RelationId child : composite.components) {
        const int32_t representative = representativeEdgeFor(child);
        if (representative < 0) continue;
        representatives.push_back(representative);
        currentSum += edges_.at(representative).size;
    }
    if (representatives.empty()) return;

    const float delta = composite.targetSum - currentSum;
    if (std::abs(delta) < 1e-4f) return;
    const int32_t compensator = representatives.back();
    edges_.at(compensator).size += delta;
    log("INVARIANT", "relación " + std::to_string(compositeId) +
                     " ajustó arista " + std::to_string(compensator));
}

bool ConstraintSolver::solve(std::vector<SemanticEdge>& compiledEdges) {
    bool resolved = true;
    for (SemanticEdge& source : compiledEdges) {
        if (source.id < 0) {
            source.id = static_cast<int32_t>(&source - compiledEdges.data());
        }
        if (relations_.find(source.geometricRelations) == relations_.end()) {
            Relation relation;
            relation.id = source.geometricRelations;
            relation.baseSize = source.transform.distance;
            relation.toleranceMode = ToleranceMode::Explicit;
            relation.toleranceValue = 50.0f;
            addRelation(relation);
        }
        addEdge(source);
    }

    for (const auto& entry : relations_) {
        if (entry.second.isRepeated) resolveDerivedTolerance(entry.first);
    }

    std::unordered_set<RelationId> propagated;
    for (const auto& entry : edges_) {
        if (isToleranceExceeded(entry.first)) {
            resolved = false;
            if (propagated.insert(entry.second.geometricRelations).second) {
                propagateConstraint(entry.second.geometricRelations);
            }
        }
    }

    for (auto it = edges_.begin(); it != edges_.end(); ++it) {
        if (it->second.merged) continue;
        for (auto jt = std::next(it); jt != edges_.end(); ++jt) {
            if (!jt->second.merged &&
                checkProximityMerge(it->first, jt->first, 1.0f)) {
                mergeEdges(it->first, jt->first);
                resolved = false;
            }
        }
    }

    for (const auto& entry : relations_) {
        if (entry.second.invariant == InvariantMode::SumPreserving) {
            enforceSumInvariant(entry.first);
        }
    }
    for (SemanticEdge& source : compiledEdges) {
        const auto it = edges_.find(source.id);
        if (it != edges_.end()) source = it->second;
    }
    return resolved;
}
