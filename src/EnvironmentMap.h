#ifndef _ENVIRONMENT_MAP_H_
#define _ENVIRONMENT_MAP_H_

#include <stdio.h>
#include <memory>
#include <vector>
#include <math.h>

#include "Transform.h"
#include "Ray.h"
#include "HDRImage.h"
#include "Material.h"
#include "DistributionSamplers.h"

class Ray; 
class RayIntersection;
class RandomNumberGenerator;

class EnvironmentMap {
    public:
        EnvironmentMap() {}
        virtual ~EnvironmentMap() {}

        virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
        virtual RGBRadianceSample sample( const Ray & ray ) const = 0;

        struct ImportanceSample {
            Ray ray;
            float pdf;
        };

        virtual ImportanceSample importanceSample( RandomNumberGenerator & rng,
                                                   const RayIntersection & intersection );
        virtual bool canImportanceSample() const { return false; }
};

class UniformEnvironmentMap : public EnvironmentMap {
    public:
        UniformEnvironmentMap() = default;
        virtual ~UniformEnvironmentMap() = default;

        virtual RGBRadianceSample sample( const Ray & ray ) const { return radiance; }

        RGBColor radiance = { 1.0f, 1.0f, 1.0f };
};

// Test pattern environment map
//
// Displays reference angles on the sphere at infinity
class TestPatternEnvironmentMap : public EnvironmentMap {
    public:
        TestPatternEnvironmentMap() {}
        virtual ~TestPatternEnvironmentMap() {}

        virtual RGBRadianceSample sample( const Ray & ray ) const;
};

class ArcLightEnvironmentMap : public EnvironmentMap {
    public:
        ArcLightEnvironmentMap();
        ArcLightEnvironmentMap( const Vector4 & d, float r );
        virtual ~ArcLightEnvironmentMap() {}

        virtual RGBRadianceSample sample( const Ray & ray ) const;

        void setLightDirection( const Vector4 & d ) { light_dir = d.normalized(); }
        void setAngularRadius( float r ) { angular_radius = r; recalculateRadiance(); }
        void setPower( float p ) { power = p; recalculateRadiance(); }

        virtual ImportanceSample importanceSample( RandomNumberGenerator & rng,
                                                   const RayIntersection & intersection );
        virtual bool canImportanceSample() const { return true; }

    protected:
        void recalculateRadiance();

        // Direction of center axis of spherical cap light
        Vector4 light_dir = Vector4( 0.0, 1.0, 0.0 );
        // Angle between center axis and outer edge of cap
        float angular_radius = M_PI / 4.0f;
        // Power in Watts across a unit spherical cap
        float power = 2.5f;

        // Cache of radiance per sample. Should be cached when parameters change
        // by calling recalculateRadiance()
        // TODO[DAC]: Colored light!
        float radiance_per_sample = 0.0f;
        float cap_solid_angle;
};

class HDRImageEnvironmentMap : public EnvironmentMap {
    public:
        HDRImageEnvironmentMap( unsigned int w, unsigned int h );
        HDRImageEnvironmentMap( const std::string & filename,
                                unsigned int w, unsigned int h );
        virtual ~HDRImageEnvironmentMap() {}

        virtual RGBRadianceSample sample( const Ray & ray ) const;

        virtual ImportanceSample importanceSample( RandomNumberGenerator & rng,
                                                   const RayIntersection & intersection );
        virtual bool canImportanceSample() const { return true; }

    protected:
        HDRImage image;
        std::unique_ptr<DistributionSampler2D> sampler;

        void updateStateFromImage();
        void maskInscribedCircle();
};

// Even illumination in all directions
//
// Displays reference angles on the sphere at infinity
class HotBoxEnvironmentMap : public EnvironmentMap {
    public:
        HotBoxEnvironmentMap(float r = 1.0f) : radiance(r) {}
        virtual ~HotBoxEnvironmentMap() {}

        virtual RGBRadianceSample sample( const Ray & ray ) const;

        float radiance = 1.0f;
};


#endif

