//
// Created by Luis Alvarez on 11/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_INHERITED_H
#define SM64_THE_MUTANT_LIMINALITY_INHERITED_H
#include "BaseCastle.h"

using SemanticId = size_t;

struct SemanticSource {
    SemanticId& semanticEdgeId;
    bool mirrored;
    bool reversed;
};

template <class T>
class Inherited {
    Scalar<T> value;
    SemanticSource src;
    void changeToClosestSemantic() {
        SemanticId closestSemanticId = src.semanticEdgeId;
        //for (SemanticId i = 0; i < BaseCastle::SEMANTIC_EDGE_COUNT; i++) {
            /*if (semanticEdge == &src.semanticEdgeId) {
                continue;
            }*/
            /*float proximity = getSemanticTensionWith(i);
            if (proximity < getSemanticTensionWith(closestSemanticId)) {
                closestSemanticId = i;
            }*/
        //}
        src.semanticEdgeId = closestSemanticId;
    }
    virtual float getSemanticTensionWith(SemanticId id); // used to calculate priority on Variables and Scalars
public:
    Inherited(SemanticSource s, Scalar<T>& v) : value(v), src(s) {}
    const T& get() const { return value; }
    SemanticSource source() const { return src; }
    /*
    void inheritFrom(const SemanticSource s, Scalar<T>& v) {
        value = v;
        //src = s;
    }*/
    float getSemanticTension() {
        changeToClosestSemantic();
        return getSemanticTensionWith(src.semanticEdgeId);
    }
};
/*
class ProportionalRelationInherited : public Inherited<Length> {
    ProportionalRelation& relation;
public:
    ProportionalRelationInherited(const SemanticSource s, const Scalar<Length>& v, const ProportionalRelation& r) : Inherited(s, v), relation(r) {}
    float getSemanticTensionWith(SemanticId id) override {
        return abs(value.get().value - relation.stableSize.get());
    }
};

// the G_ are global variables that can change on runtime to compensate certain semantic tensions, and are used to weight the importance of certain semantic properties over others.
class SeparationAngleInherited : public Inherited<Theta> {
    Theta& theta;
public:
    SeparationAngleInherited(const SemanticSource s, const Theta& r) : Inherited(s, baseCastle[s.semanticEdgeId].orientation), theta(r) {};
    float getSemanticTensionWith(SemanticId id) override { // Make sure you've recalculated nearest semantic edge for this property before calculating tension, or else it will return a higher tension than it should be
        int e = INT_MAX;
        int e2 = INT_MIN;
        int t = (baseCastle[id].angularSeparation/(2*M_PI))*256;
        if (t == 0) {
            throw NonRecoverableContradictionException("SeparationAngleInherited: stableSeparation is zero, cannot measure semantic tension.");
        }
        t = t&-t;
        e = log2(t);
        t = (value.get().value/(2*M_PI))*256;
        if (t == 0) {
            throw NonRecoverableContradictionException("SeparationAngleInherited: value is zero, cannot measure semantic tension.");
        }
        t = t&-t;
        e2 = log2(t);
        return ((abs(float(e) - float(e2)) * G_GeneralBinaryDivisionImportance) + (std::abs((baseCastle[id].angularSeparation/(2*M_PI)) - (value.get().value/(2*M_PI))) * G_GeneralAngularDifferenceImportance)/(1 << std::max(e, e2))) * G_GeneralSeparationAngleTensionImportance;
    }
};

class TextureInherited : public Inherited<EnumIndex> {
    EnumIndex& texture;
public:
    TextureInherited(const SemanticSource s, const Scalar<EnumIndex>& v, const EnumIndex& r) : Inherited(s, v), texture(r) {};
    float getSemanticTensionWith(SemanticId id) override {
        // make a semantic tension metric based on texture similarity
    }
};*/


#endif // SM64_THE_MUTANT_LIMINALITY_INHERITED_H
