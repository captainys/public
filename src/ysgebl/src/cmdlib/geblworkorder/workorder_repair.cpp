#include "geblworkorder.h"
#include <ysshellext_repairutil.h>



YSRESULT GeblCmd_WorkOrder::RunRepairWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(2<=args.GetN())
	{
		auto subCmd=args[1];
		if(0==subCmd.STRCMP("DELETE"))
		{
			return RunRepairWorkOrder_Delete(workOrder,args);
		}

		YsString errorReason;
		errorReason.Printf("Unrecognized sub command [%s]",args[1].Txt());
		ShowError(workOrder,errorReason);
	}
	else
	{
		ShowError(workOrder,"Sub-command not given.");
	}
	return YSERR;
}
YSRESULT GeblCmd_WorkOrder::RunRepairWorkOrder_Delete(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(3<=args.GetN())
	{
		auto subCmd=args[2];
		if(0==subCmd.STRCMP("IDENTICAL_POLYGON"))
		{
			return RunRepairWorkOrder_Delete_IdenticalPolygon(workOrder,args);
		}
		else if(0==subCmd.STRCMP("FLAT_TRIANGLE"))
		{
			return RunRepairWorkOrder_Delete_FlatTriangle(workOrder,args);
		}
		YsString errorReason;
		errorReason.Printf("Unrecognized sub-sub command [%s]",args[1].Txt());
		ShowError(workOrder,errorReason);
	}
	else
	{
		ShowError(workOrder,"Sub-Sub-command not given.");
	}
	return YSERR;
}
YSRESULT GeblCmd_WorkOrder::RunRepairWorkOrder_Delete_IdenticalPolygon(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	YsShellExt_IdenticalPolygonRemover remover;
	remover.MakeDuplicatePolygonList(slHd->Conv());
	remover.DeleteDuplicatePolygon(*slHd);
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunRepairWorkOrder_Delete_FlatTriangle(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(5<=args.size())
	{
		YsShellExt_FlatTriangleKiller::FLAT_TRIANGLE_CLASSIFICATION classifier;
		if(0==args[3].STRCMP("cannot_calc_normal"))
		{
			classifier=YsShellExt_FlatTriangleKiller::CANNOT_COMPUTE_NORMAL;
		}
		else if(0==args[3].STRCMP("narrower_than_tolerance"))
		{
			classifier=YsShellExt_FlatTriangleKiller::NARROWER_THAN_TOLERANCE;
		}
		else
		{
			ShowError(workOrder,"Unknown flat-triangle criterion.");
			return YSERR;
		}

		YsArray <YsShellExt_FlatTriangleKiller::REPAIR_OPTION> opt;
		for(YSSIZE_T i=4; i<args.size(); ++i)
		{
			if(0==args[i].STRCMP("collapse_tol"))
			{
				opt.push_back(YsShellExt_FlatTriangleKiller::COLLAPSE_SHORTER_THAN_TOLERANCE);
			}
			else if(0==args[i].STRCMP("swap"))
			{
				opt.push_back(YsShellExt_FlatTriangleKiller::SWAPPING_LONGEST_EDGE);
			}
			else if(0==args[i].STRCMP("merge"))
			{
				opt.push_back(YsShellExt_FlatTriangleKiller::MERGE_LONGEST_EDGE);
			}
			else
			{
				ShowError(workOrder,"Unknown repair scheme.");
				return YSERR;
			}
		}

		YsShellExt_FlatTriangleKiller killer;
		killer.classifier=classifier;
		killer.repairOption=opt;
		killer.Apply(*slHd);
		return YSOK;
	}
	else
	{
		ShowError(workOrder,"Too few arguments.");
	}
	return YSERR;
}
