/* AUTO GENERATED FILE.  DO NOT EDIT */

/* Should be updated by update_register_all_functions script when new functions are added */

#include "libfunction_precompiled.h"

#include "register_all_functions.h"

#include "function_boilerplate.h"
REGISTER_DCL(FunctionComposePair);
REGISTER_DCL(FunctionComposeTriple);
REGISTER_DCL(FunctionConstant);
REGISTER_DCL(FunctionIdentity);
REGISTER_DCL(FunctionPostTransform);
REGISTER_DCL(FunctionPreTransform);
REGISTER_DCL(FunctionTop);
REGISTER_DCL(FunctionTransform);
REGISTER_DCL(FunctionTransformGeneralised);
REGISTER_DCL(FunctionAdd);
REGISTER_DCL(FunctionMultiply);
REGISTER_DCL(FunctionDivide);
REGISTER_DCL(FunctionMax);
REGISTER_DCL(FunctionMin);
REGISTER_DCL(FunctionModulus);
REGISTER_DCL(FunctionExp);
REGISTER_DCL(FunctionSin);
REGISTER_DCL(FunctionCos);
REGISTER_DCL(FunctionChooseStrip);
REGISTER_DCL(FunctionChooseStripBlend);
REGISTER_DCL(FunctionChooseSphere);
REGISTER_DCL(FunctionChooseRect);
REGISTER_DCL(FunctionChooseFrom2InCubeMesh);
REGISTER_DCL(FunctionChooseFrom3InCubeMesh);
REGISTER_DCL(FunctionChooseFrom2InSquareGrid);
REGISTER_DCL(FunctionChooseFrom3InSquareGrid);
REGISTER_DCL(FunctionChooseFrom2InTriangleGrid);
REGISTER_DCL(FunctionChooseFrom3InTriangleGrid);
REGISTER_DCL(FunctionChooseFrom3InDiamondGrid);
REGISTER_DCL(FunctionChooseFrom3InHexagonGrid);
REGISTER_DCL(FunctionChooseFrom2InBorderedHexagonGrid);
REGISTER_DCL(FunctionFilter2D);
REGISTER_DCL(FunctionFilter3D);
REGISTER_DCL(FunctionAverageSamples);
REGISTER_DCL(FunctionStreak);
REGISTER_DCL(FunctionAverageRing);
REGISTER_DCL(FunctionFilterRing);
REGISTER_DCL(FunctionConvolveSamples);
REGISTER_DCL(FunctionAccumulateOctaves);
REGISTER_DCL(FunctionFriezeGroupHopFreeZ);
REGISTER_DCL(FunctionFriezeGroupHopClampZ);
REGISTER_DCL(FunctionFriezeGroupHopWarpFreeZ);
REGISTER_DCL(FunctionFriezeGroupHopWarpClampZ);
REGISTER_DCL(FunctionFriezeGroupJumpFreeZ);
REGISTER_DCL(FunctionFriezeGroupJumpClampZ);
REGISTER_DCL(FunctionFriezeGroupSidle);
REGISTER_DCL(FunctionFriezeGroupSpinhop);
REGISTER_DCL(FunctionFriezeGroupSpinjump);
REGISTER_DCL(FunctionFriezeGroupSpinsidle);
REGISTER_DCL(FunctionFriezeGroupStep);
REGISTER_DCL(FunctionCross);
REGISTER_DCL(FunctionGeometricInversion);
REGISTER_DCL(FunctionReflect);
REGISTER_DCL(FunctionDerivative);
REGISTER_DCL(FunctionDerivativeGeneralised);
REGISTER_DCL(FunctionGradient);
REGISTER_DCL(FunctionGradientGeneralised);
REGISTER_DCL(FunctionDivergence);
REGISTER_DCL(FunctionCurl);
REGISTER_DCL(FunctionScalarLaplacian);
REGISTER_DCL(FunctionMandelbrotChoose);
REGISTER_DCL(FunctionMandelbrotContour);
REGISTER_DCL(FunctionJuliaChoose);
REGISTER_DCL(FunctionJuliaContour);
REGISTER_DCL(FunctionJuliabrotChoose);
REGISTER_DCL(FunctionJuliabrotContour);
REGISTER_DCL(FunctionKaleidoscope);
REGISTER_DCL(FunctionKaleidoscopeZRotate);
REGISTER_DCL(FunctionKaleidoscopeTwist);
REGISTER_DCL(FunctionWindmill);
REGISTER_DCL(FunctionWindmillZRotate);
REGISTER_DCL(FunctionWindmillTwist);
REGISTER_DCL(FunctionMagnitudes);
REGISTER_DCL(FunctionMagnitude);
REGISTER_DCL(FunctionCone);
REGISTER_DCL(FunctionExpCone);
REGISTER_DCL(FunctionSeparateZ);
REGISTER_DCL(FunctionIterate);
REGISTER_DCL(FunctionNoiseOneChannel);
REGISTER_DCL(FunctionMultiscaleNoiseOneChannel);
REGISTER_DCL(FunctionNoiseThreeChannel);
REGISTER_DCL(FunctionMultiscaleNoiseThreeChannel);
REGISTER_DCL(FunctionOrthoSphereShaded);
REGISTER_DCL(FunctionOrthoSphereShadedBumpMapped);
REGISTER_DCL(FunctionOrthoSphereReflect);
REGISTER_DCL(FunctionOrthoSphereReflectBumpMapped);
REGISTER_DCL(FunctionShadow);
REGISTER_DCL(FunctionShadowGeneralised);
REGISTER_DCL(FunctionCartesianToSpherical);
REGISTER_DCL(FunctionSphericalToCartesian);
REGISTER_DCL(FunctionEvaluateInSpherical);
REGISTER_DCL(FunctionSpiralLinear);
REGISTER_DCL(FunctionSpiralLogarithmic);
REGISTER_DCL(FunctionIsotropicScale);
REGISTER_DCL(FunctionPreTransformGeneralised);
REGISTER_DCL(FunctionPostTransformGeneralised);
REGISTER_DCL(FunctionTransformQuadratic);
REGISTER_DCL(FunctionRotate);

void register_all_functions(FunctionRegistry& r)
{
  register_FunctionComposePair(r);
  register_FunctionComposeTriple(r);
  register_FunctionConstant(r);
  register_FunctionIdentity(r);
  register_FunctionPostTransform(r);
  register_FunctionPreTransform(r);
  register_FunctionTop(r);
  register_FunctionTransform(r);
  register_FunctionTransformGeneralised(r);
  register_FunctionAdd(r);
  register_FunctionMultiply(r);
  register_FunctionDivide(r);
  register_FunctionMax(r);
  register_FunctionMin(r);
  register_FunctionModulus(r);
  register_FunctionExp(r);
  register_FunctionSin(r);
  register_FunctionCos(r);
  register_FunctionChooseStrip(r);
  register_FunctionChooseStripBlend(r);
  register_FunctionChooseSphere(r);
  register_FunctionChooseRect(r);
  register_FunctionChooseFrom2InCubeMesh(r);
  register_FunctionChooseFrom3InCubeMesh(r);
  register_FunctionChooseFrom2InSquareGrid(r);
  register_FunctionChooseFrom3InSquareGrid(r);
  register_FunctionChooseFrom2InTriangleGrid(r);
  register_FunctionChooseFrom3InTriangleGrid(r);
  register_FunctionChooseFrom3InDiamondGrid(r);
  register_FunctionChooseFrom3InHexagonGrid(r);
  register_FunctionChooseFrom2InBorderedHexagonGrid(r);
  register_FunctionFilter2D(r);
  register_FunctionFilter3D(r);
  register_FunctionAverageSamples(r);
  register_FunctionStreak(r);
  register_FunctionAverageRing(r);
  register_FunctionFilterRing(r);
  register_FunctionConvolveSamples(r);
  register_FunctionAccumulateOctaves(r);
  register_FunctionFriezeGroupHopFreeZ(r);
  register_FunctionFriezeGroupHopClampZ(r);
  register_FunctionFriezeGroupHopWarpFreeZ(r);
  register_FunctionFriezeGroupHopWarpClampZ(r);
  register_FunctionFriezeGroupJumpFreeZ(r);
  register_FunctionFriezeGroupJumpClampZ(r);
  register_FunctionFriezeGroupSidle(r);
  register_FunctionFriezeGroupSpinhop(r);
  register_FunctionFriezeGroupSpinjump(r);
  register_FunctionFriezeGroupSpinsidle(r);
  register_FunctionFriezeGroupStep(r);
  register_FunctionCross(r);
  register_FunctionGeometricInversion(r);
  register_FunctionReflect(r);
  register_FunctionDerivative(r);
  register_FunctionDerivativeGeneralised(r);
  register_FunctionGradient(r);
  register_FunctionGradientGeneralised(r);
  register_FunctionDivergence(r);
  register_FunctionCurl(r);
  register_FunctionScalarLaplacian(r);
  register_FunctionMandelbrotChoose(r);
  register_FunctionMandelbrotContour(r);
  register_FunctionJuliaChoose(r);
  register_FunctionJuliaContour(r);
  register_FunctionJuliabrotChoose(r);
  register_FunctionJuliabrotContour(r);
  register_FunctionKaleidoscope(r);
  register_FunctionKaleidoscopeZRotate(r);
  register_FunctionKaleidoscopeTwist(r);
  register_FunctionWindmill(r);
  register_FunctionWindmillZRotate(r);
  register_FunctionWindmillTwist(r);
  register_FunctionMagnitudes(r);
  register_FunctionMagnitude(r);
  register_FunctionCone(r);
  register_FunctionExpCone(r);
  register_FunctionSeparateZ(r);
  register_FunctionIterate(r);
  register_FunctionNoiseOneChannel(r);
  register_FunctionMultiscaleNoiseOneChannel(r);
  register_FunctionNoiseThreeChannel(r);
  register_FunctionMultiscaleNoiseThreeChannel(r);
  register_FunctionOrthoSphereShaded(r);
  register_FunctionOrthoSphereShadedBumpMapped(r);
  register_FunctionOrthoSphereReflect(r);
  register_FunctionOrthoSphereReflectBumpMapped(r);
  register_FunctionShadow(r);
  register_FunctionShadowGeneralised(r);
  register_FunctionCartesianToSpherical(r);
  register_FunctionSphericalToCartesian(r);
  register_FunctionEvaluateInSpherical(r);
  register_FunctionSpiralLinear(r);
  register_FunctionSpiralLogarithmic(r);
  register_FunctionIsotropicScale(r);
  register_FunctionPreTransformGeneralised(r);
  register_FunctionPostTransformGeneralised(r);
  register_FunctionTransformQuadratic(r);
  register_FunctionRotate(r);
}
