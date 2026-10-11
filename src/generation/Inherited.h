//
// Created by Luis Alvarez on 11/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_INHERITED_H
#define SM64_THE_MUTANT_LIMINALITY_INHERITED_H
#include "SemanticEdge.h"

using SemanticId = size_t;

SemanticEdge baseCastle[64] = SemanticEdge{}; // stub!

struct SemanticSource {
    SemanticId& semanticEdgeId;
    bool mirrored;
    bool reversed;
};

template <class T>
class Inherited {
    Scalar<T>& value;
    SemanticSource src;
public:
    Inherited(SemanticSource s, Scalar<T>& v) : value(v), src(s) {}
    const T& get() const { return value; }
    SemanticSource source() const { return src; }
    void inheritFrom(const SemanticSource s, Scalar<T>& v) {
        value = v;
        src = s;
    }
    virtual float getSemanticTension(SemanticId id); // used to calculate priority on Variables and Scalars
    void changeToClosestSemantic() {
        SemanticId closestSemanticId = src.semanticEdgeId;
        for (SemanticId i = 0; i < sizeof(baseCastle) / sizeof(SemanticEdge); i++) {
            /*if (semanticEdge == &src.semanticEdgeId) {
                continue;
            }*/
            float proximity = getSemanticTension(i);
            if (proximity < getSemanticTension(closestSemanticId)) {
                closestSemanticId = i;
            }
        }
    }
};

class ProportionalRelationInherited : public Inherited<Length> {
    ProportionalRelation& relation;
public:
    ProportionalRelationInherited(const SemanticSource s, const Scalar<Length>& v, const ProportionalRelation& r) : Inherited(s, v), relation(r) {}
    float getSemanticTension(SemanticId id) override {
        return abs(value.get().value - relation.stableSize.get());
    }
};

class SeparationAngleInherited : public Inherited<Theta> {
    Theta& theta;
public:
    SeparationAngleInherited(const SemanticSource s, const Scalar<Theta>& v, const Theta& r) : Inherited(s, v), theta(r) {};
    float getSemanticTension(SemanticId id) override {
        if (value.get().value != baseCastle[id].stableSeparation) {
            return INFINITY;
        } else {
            return 0.0f;
        }
    }
};

class TextureInherited : public Inherited<EnumIndex> {
    EnumIndex& texture;
public:
    TextureInherited(const SemanticSource s, const Scalar<EnumIndex>& v, const EnumIndex& r) : Inherited(s, v), texture(r) {};
    float getSemanticTension(SemanticId id) override {
        // make a semantic tension metric based on texture similarity
    }
};


#endif // SM64_THE_MUTANT_LIMINALITY_INHERITED_H
