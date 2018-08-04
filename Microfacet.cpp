#include <algorithm>
#include <cassert>
#include <cmath>

#include "Microfacet.h"

namespace GeometryShadowing {

float Implicit(float NdV, float NdL)
{
    return NdV * NdL;
}

float CookTorrance(float NdH, float NdV, float NdL, float VdH)
{
    float G1 = 2.0f * NdH * NdV / VdH;
    float G2 = 2.0f * NdH * NdL / VdH;
    return std::min(1.0f, std::min(G1, G2));
}

}; // GeometryShadowing

namespace NormalDistribution {

float Beckman(float roughness, float NdH)
{
    float m2 = roughness * roughness;
    float NdH2 = NdH * NdH;
    float NdH4 = NdH2 * NdH2;

    assert(roughness > 0.0f && m2 > 0.0f && NdH2 > 0.0f && NdH4 > 0.0f);

    float x = (NdH2 - 1) / (m2 * NdH2);
    float num = std::exp(x);
    float den = M_PI * m2 * NdH4;

    return num / den;
}

float BlinnPhong(float roughness, float NdH)
{
    float m2 = roughness * roughness;
    float NdH2 = NdH * NdH;
    float NdH4 = NdH2 * NdH2;

    assert(m2 > 0.0f);

    float c = 1.0f / (m2 * M_PI);
    float e = 2 / m2 - 2;
    float D = c * std::pow(NdH, e);

    return D;
}

}; // NormalDistribution


