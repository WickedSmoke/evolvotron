/* AUTO GENERATED FILE.  DO NOT EDIT */

/* Should be updated by update_register_all_functions script when new functions are added */

#include "register_all_functions.h"

#include "function_compose_pair.h"
#include "function_compose_triple.h"
#include "function_core.h"
#include "function_null.h"
#include "function_post_transform.h"
#include "function_pre_transform.h"
#include "function_top.h"
#include "function_transform_generalised.h"
#include "functions.h"
#include "functions_friezegroup_hop.h"
#include "functions_friezegroup_spinhop.h"
#include "functions_noise.h"

void register_all_functions(FunctionRegistry& r)
{
  REGISTER(r,FunctionComposePair);
  REGISTER(r,FunctionComposeTriple);
  REGISTER(r,FunctionConstant);
  REGISTER(r,FunctionIdentity);
  REGISTER(r,FunctionTransform);
  REGISTER(r,FunctionNull);
  REGISTER(r,FunctionPostTransform);
  REGISTER(r,FunctionPreTransform);
  REGISTER(r,FunctionTop);
  REGISTER(r,FunctionTransformGeneralised);
  REGISTER(r,FunctionIsotropicScale);
  REGISTER(r,FunctionPreTransformGeneralised);
  REGISTER(r,FunctionPostTransformGeneralised);
  REGISTER(r,FunctionTransformQuadratic);
  REGISTER(r,FunctionCartesianToSpherical);
  REGISTER(r,FunctionSphericalToCartesian);
  REGISTER(r,FunctionEvaluateInSpherical);
  REGISTER(r,FunctionRotate);
  REGISTER(r,FunctionSin);
  REGISTER(r,FunctionCos);
  REGISTER(r,FunctionSpiralLinear);
  REGISTER(r,FunctionSpiralLogarithmic);
  REGISTER(r,FunctionDerivative);
  REGISTER(r,FunctionDerivativeGeneralised);
  REGISTER(r,FunctionGradient);
  REGISTER(r,FunctionGradientGeneralised);
  REGISTER(r,FunctionDivergence);
  REGISTER(r,FunctionCurl);
  REGISTER(r,FunctionScalarLaplacian);
  REGISTER(r,FunctionAdd);
  REGISTER(r,FunctionMultiply);
  REGISTER(r,FunctionDivide);
  REGISTER(r,FunctionCross);
  REGISTER(r,FunctionMax);
  REGISTER(r,FunctionMin);
  REGISTER(r,FunctionModulus);
  REGISTER(r,FunctionExp);
  REGISTER(r,FunctionGeometricInversion);
  REGISTER(r,FunctionReflect);
  REGISTER(r,FunctionKaleidoscope);
  REGISTER(r,FunctionKaleidoscopeZRotate);
  REGISTER(r,FunctionKaleidoscopeTwist);
  REGISTER(r,FunctionWindmill);
  REGISTER(r,FunctionWindmillZRotate);
  REGISTER(r,FunctionWindmillTwist);
  REGISTER(r,FunctionStrip);
  REGISTER(r,FunctionStripBlend);
  REGISTER(r,FunctionFriezeGroupJump);
  REGISTER(r,FunctionFriezeGroupJumpWarped);
  REGISTER(r,FunctionFriezeGroupJumpFreeZ);
  REGISTER(r,FunctionFriezeGroupJumpWarpedFreeZ);
  REGISTER(r,FunctionFriezeGroupSidle);
  REGISTER(r,FunctionFriezeGroupSidleFreeZ);
  REGISTER(r,FunctionFriezeGroupStep);
  REGISTER(r,FunctionFriezeGroupStepWarped);
  REGISTER(r,FunctionFriezeGroupStepFreeZ);
  REGISTER(r,FunctionFriezeGroupStepWarpedFreeZ);
  REGISTER(r,FunctionFriezeGroupSpinjump);
  REGISTER(r,FunctionFriezeGroupSpinjumpFreeZ);
  REGISTER(r,FunctionFriezeGroupSpinsidle);
  REGISTER(r,FunctionFriezeGroupSpinsidleWarped);
  REGISTER(r,FunctionFriezeGroupSpinsidleFreeZ);
  REGISTER(r,FunctionFriezeGroupSpinsidleWarpedFreeZ);
  REGISTER(r,FunctionMagnitudes);
  REGISTER(r,FunctionMagnitude);
  REGISTER(r,FunctionChooseSphere);
  REGISTER(r,FunctionChooseRect);
  REGISTER(r,FunctionChooseFrom2InCubeMesh);
  REGISTER(r,FunctionChooseFrom3InCubeMesh);
  REGISTER(r,FunctionChooseFrom2InSquareGrid);
  REGISTER(r,FunctionChooseFrom3InSquareGrid);
  REGISTER(r,FunctionChooseFrom2InTriangleGrid);
  REGISTER(r,FunctionChooseFrom3InTriangleGrid);
  REGISTER(r,FunctionChooseFrom3InDiamondGrid);
  REGISTER(r,FunctionChooseFrom3InHexagonGrid);
  REGISTER(r,FunctionChooseFrom2InBorderedHexagonGrid);
  REGISTER(r,FunctionOrthoSphereShaded);
  REGISTER(r,FunctionOrthoSphereShadedBumpMapped);
  REGISTER(r,FunctionOrthoSphereReflect);
  REGISTER(r,FunctionOrthoSphereReflectBumpMapped);
  REGISTER(r,FunctionFilter2D);
  REGISTER(r,FunctionFilter3D);
  REGISTER(r,FunctionShadow);
  REGISTER(r,FunctionShadowGeneralised);
  REGISTER(r,FunctionCone);
  REGISTER(r,FunctionExpCone);
  REGISTER(r,FunctionSeparateZ);
  REGISTER(r,FunctionIterate);
  REGISTER(r,FunctionAverageSamples);
  REGISTER(r,FunctionStreak);
  REGISTER(r,FunctionAverageRing);
  REGISTER(r,FunctionFilterRing);
  REGISTER(r,FunctionConvolveSamples);
  REGISTER(r,FunctionAccumulateOctaves);
  REGISTER(r,FunctionMandelbrotChoose);
  REGISTER(r,FunctionMandelbrotContour);
  REGISTER(r,FunctionJuliaChoose);
  REGISTER(r,FunctionJuliaContour);
  REGISTER(r,FunctionJuliabrotChoose);
  REGISTER(r,FunctionJuliabrotContour);
  REGISTER(r,FunctionFriezeGroupHop);
  REGISTER(r,FunctionFriezeGroupHopBlend);
  REGISTER(r,FunctionFriezeGroupHopWarped);
  REGISTER(r,FunctionFriezeGroupHopFreeZ);
  REGISTER(r,FunctionFriezeGroupHopBlendFreeZ);
  REGISTER(r,FunctionFriezeGroupHopWarpedFreeZ);
  REGISTER(r,FunctionFriezeGroupSpinhop);
  REGISTER(r,FunctionFriezeGroupSpinhopWarped);
  REGISTER(r,FunctionFriezeGroupSpinhopFreeZ);
  REGISTER(r,FunctionFriezeGroupSpinhopWarpedFreeZ);
  REGISTER(r,FunctionNoiseOneChannel);
  REGISTER(r,FunctionMultiscaleNoiseOneChannel);
  REGISTER(r,FunctionNoiseThreeChannel);
  REGISTER(r,FunctionMultiscaleNoiseThreeChannel);
}
