#include <gtest/gtest.h>
#include "FastRender.h"

namespace {

// Hemisphere direction generator, useful for easily iterating over directions in the hemisphere
class HemisphereDirectionGenerator
{
    public:
        HemisphereDirectionGenerator(unsigned int nThetaSteps, unsigned int nPhiSteps)
            : numThetaSteps(nThetaSteps), numPhiSteps(nPhiSteps) {}

        // iteration
        void reset() { currentIndex = 0; }
        void next() { currentIndex++; thetaIndex = currentIndex / numPhiSteps; phiIndex = currentIndex % numPhiSteps; }
        bool done() const { return currentIndex >= numThetaSteps * numPhiSteps; }

        // state of current iteration step
        float theta()     const { return ((float) thetaIndex + 0.5) / numThetaSteps * M_PI * 0.5; }
        float phi()       const { return ((float) phiIndex + 0.5) / numPhiSteps * 2.0 * M_PI; }

        float x()         const { return cosf(phi()) * sinf(theta()); }
        float y()         const { return sinf(phi()) * sinf(theta()); }
        float z()         const { return cosf(theta()); }
        Vector4 vector()  const { return Vector4(x(), y(), z()); }

        float dtheta()    const { return 0.5 * M_PI / (float) numThetaSteps; }
        float dphi()      const { return 2.0 * M_PI / (float) numPhiSteps; }
        float dArea()     const { return dtheta() * dphi() * sinf(theta()); }
        float dAreaProj() const { return dArea() * cosf(theta()); }

    protected:
        unsigned int currentIndex = 0;
        unsigned int thetaIndex = 0;
        unsigned int phiIndex = 0;
        unsigned int numThetaSteps;
        unsigned int numPhiSteps;
};

bool bxdfIsNonNegative(const Material & material)
{
    const unsigned int numThetaSteps = 25;
    const unsigned int numPhiSteps = 100;
    Vector4 normal(0, 0, 1);
    bool success = true;

    HemisphereDirectionGenerator dirGen1(numThetaSteps, numPhiSteps);
    HemisphereDirectionGenerator dirGen2(numThetaSteps, numPhiSteps);

    for(; !dirGen1.done(); dirGen1.next()) {
        Vector4 wi = dirGen1.vector();
        for(; !dirGen2.done(); dirGen2.next()) {
            Vector4 wo = dirGen2.vector();
            float bxdfValue = material.BxDF(normal, wi, wo);

            if(bxdfValue < 0.0f) {
                std::cout << "BxDF is negative for " << wi << ", " << wo << std::endl;
            }
            success = success && (bxdfValue >= 0.0f);
        }
    }

    return success;
}

bool bxdfObeysHelmholtzReciprocity(const Material & material)
{
    const unsigned int numThetaSteps = 25;
    const unsigned int numPhiSteps = 100;
    Vector4 normal(0, 0, 1);
    bool success = true;
    const float bxdfCompareEpsilon = 0.0001f;

    HemisphereDirectionGenerator dirGen1(numThetaSteps, numPhiSteps);
    HemisphereDirectionGenerator dirGen2(numThetaSteps, numPhiSteps);

    for(; !dirGen1.done(); dirGen1.next()) {
        Vector4 wi = dirGen1.vector();
        for(; !dirGen2.done(); dirGen2.next()) {
            Vector4 wo = dirGen2.vector();
            float bxdfFwd = material.BxDF(normal, wi, wo);
            float bxdfRev = material.BxDF(normal, wo, wi);
            float absdiff = fabs(bxdfFwd - bxdfRev);

            if(absdiff > bxdfCompareEpsilon) {
                std::cout << "Helmholtz reciprocity violated for " << wi << ", " << wo
                    << " : fwd " << bxdfFwd << " rev " << bxdfRev << std::endl;
            }
            success = success && (fabs(bxdfFwd - bxdfRev) <= bxdfCompareEpsilon);
        }
    }

    return success;
}

bool bxdfConservesEnergyForOutDirection(const Material & material, const Vector4 & wo)
{
    const unsigned int numThetaSteps = 100;
    const unsigned int numPhiSteps = 400;
    const float integralEpsilon = 0.01;
    Vector4 normal(0, 0, 1);
    float areaSum = 0.0;
    float integral = 0.0;

    HemisphereDirectionGenerator dirGen(numThetaSteps, numPhiSteps);

    for(; !dirGen.done(); dirGen.next()) {
        Vector4 wi = dirGen.vector();
        //std::cout << wi << std::endl;
        float bxdfValue = material.BxDF(normal, wi, wo);
        integral += bxdfValue * dirGen.dAreaProj();
        areaSum += dirGen.dArea();
    }

    //std::string name = material.name();
    //printf("%s integral(BxDF*dAreaProj) = %f\n", name.c_str(), integral);

    if(integral > 1.0f + integralEpsilon) {
        std::string name = material.name();
        printf("%s integral(BxDF*dAreaProj) = %f should integrate to <= 1.0\n", name.c_str(), integral);
    }

    return integral <= 1.0f + integralEpsilon;
}

bool bxdfConservesEnergy(const Material & material)
{
    const unsigned int numThetaSteps = 16;
    const unsigned int numPhiSteps = 32;
    bool success = true;

    HemisphereDirectionGenerator dirGen(numThetaSteps, numPhiSteps);

    for(; !dirGen.done(); dirGen.next()) {
        Vector4 wo = dirGen.vector();
        //std::cout << wo << std::endl;
        success = success && bxdfConservesEnergyForOutDirection(material, wo);
    }

    return success;
}

// Materials
const auto diffuseWhite             = DiffuseMaterial(1.0, 1.0, 1.0);
const auto diffuseBlack             = DiffuseMaterial(0.0, 0.0, 0.0);
const auto diffuseRed               = DiffuseMaterial(1.0, 0.0, 0.0);
const auto diffuseGreen             = DiffuseMaterial(0.0, 1.0, 0.0);
const auto diffuseBlue              = DiffuseMaterial(0.0, 0.0, 1.0);
const auto diffuseCheckerBW         = DiffuseCheckerBoardMaterial(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
const auto diffuseUV                = DiffuseUVMaterial();
const auto mirror                   = MirrorMaterial();
const auto cookTorranceDefaultWhite = CookTorranceMaterial(1.0, 1.0, 1.0);

TEST(ValidateBxDFNonNegative, IsNonNegative) {
    EXPECT_TRUE(bxdfIsNonNegative(diffuseWhite));
    EXPECT_TRUE(bxdfIsNonNegative(diffuseBlack));
    EXPECT_TRUE(bxdfIsNonNegative(diffuseRed));
    EXPECT_TRUE(bxdfIsNonNegative(diffuseGreen));
    EXPECT_TRUE(bxdfIsNonNegative(diffuseBlue));
    EXPECT_TRUE(bxdfIsNonNegative(diffuseCheckerBW));
    EXPECT_TRUE(bxdfIsNonNegative(diffuseUV));
    EXPECT_TRUE(bxdfIsNonNegative(mirror));
    EXPECT_TRUE(bxdfIsNonNegative(cookTorranceDefaultWhite));
}

TEST(ValidateBxDFHelmholtzReciprocity, IsNonNegative) {
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(diffuseWhite));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(diffuseBlack));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(diffuseRed));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(diffuseGreen));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(diffuseBlue));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(diffuseCheckerBW));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(diffuseUV));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(mirror));
    EXPECT_TRUE(bxdfObeysHelmholtzReciprocity(cookTorranceDefaultWhite));
}

TEST(ValidateBxDFConservesEnergy, IsNonNegative) {
    EXPECT_TRUE(bxdfConservesEnergy(diffuseWhite));
    EXPECT_TRUE(bxdfConservesEnergy(diffuseBlack));
    EXPECT_TRUE(bxdfConservesEnergy(diffuseRed));
    EXPECT_TRUE(bxdfConservesEnergy(diffuseGreen));
    EXPECT_TRUE(bxdfConservesEnergy(diffuseBlue));
    EXPECT_TRUE(bxdfConservesEnergy(diffuseCheckerBW));
    EXPECT_TRUE(bxdfConservesEnergy(diffuseUV));
    EXPECT_TRUE(bxdfConservesEnergy(mirror));
    EXPECT_TRUE(bxdfConservesEnergy(cookTorranceDefaultWhite));
}

} // namespace

int main(int argc, char **argv) {
      ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

