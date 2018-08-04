#ifndef _MICROFACET_H_
#define _MICROFACET_H_

namespace GeometryShadowing {

float Implicit(float NdV, float NdL);
float CookTorrance(float NdH, float NdV, float NdL, float VdH);

};

namespace NormalDistribution {

float Beckman(float roughness, float NdH);
float BlinnPhong(float roughness, float NdH);

};


#endif
