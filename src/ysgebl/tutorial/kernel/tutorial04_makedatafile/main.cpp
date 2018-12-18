#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellext_primitiveshapeutil.h>

int main(void)
{
	YsShellExt shl;
	YsShellExt_SphereGeneratorUtil sphereUtil;
	sphereUtil.SetUp(YsVec3::Origin(),5.0,32,32);
	sphereUtil.Generate(shl);
	YsShellExt_BoxGeneratorUtil boxUtil;
	boxUtil.SetUp(YsVec3(-4.0,-4.0,-4.0),YsVec3(4.0,4.0,4.0));
	boxUtil.Generate(shl);
	YsShellExt_SaveGeneral("selfitsc.srf",shl);
	return 0;
}
