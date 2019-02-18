/* ////////////////////////////////////////////////////////////

File Name: workorder.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include "geblworkorder.h"
#include "../geblcmdparam/cmdparam.h"


void GeblCmd_WorkOrder::SetCurrentShellGroup(YsShellDnmContainer <YsShellExtEdit> &shlGrp)
{
	this->shlGrpPtr=&shlGrp;
}
void GeblCmd_WorkOrder::SetCurrentShell(YsShellDnmContainer <YsShellExtEdit>::Node *slHd)
{
	this->slHd=slHd;
}

YSRESULT GeblCmd_WorkOrder::RunWorkOrder(const YsString &workOrder)
{
	printf("RUNNING>> %s\n",workOrder.Txt());

	YsArray <YsString,16> args;
	if(YSOK==workOrder.Arguments(args) && 0<args.GetN())
	{
		args[0].Capitalize();
		if(0==strcmp(args[0],"REM") || '#'==args[0][0])
		{
			return YSOK; // Remark.  Nothing to do.
		}
		if(0==args[0].Strcmp("CONFIG"))
		{
			return RunConfigWorkOrder(workOrder,args);
		}
		if(0==strcmp(args[0],"CONSTEDGE"))
		{
			return RunConstEdgeWorkOrder(workOrder,args);
		}
		if(0==args[0].STRCMP("FACEGROUP"))
		{
			return RunFaceGroupWorkOrder(workOrder,args);
		}
		if(0==strcmp(args[0],"MERGEVERTEX"))
		{
			return RunMergeVertexWorkOrder(workOrder,args);
		}
		if(0==strcmp(args[0],"STITCH"))
		{
			return RunStitchWorkOrder(workOrder,args);
		}
		if(0==strcmp(args[0],"ORIENTATION"))
		{
			return RunOrientationWorkOrder(workOrder,args);
		}
		if(0==strcmp(args[0],"GLOBAL"))
		{
			return RunGlobalWorkOrder(workOrder,args);
		}
		if(0==strcmp(args[0],"SHOWHELP"))
		{
			ShowHelp();
			return YSOK;
		}
		if(0==args[0].STRCMP("FILEIO"))
		{
			return RunFileIOWorkOrder(workOrder,args);
		}
		if(0==args[0].STRCMP("CONTROL"))
		{
			return RunControlWorkOrder(workOrder,args);
		}
		if(0==args[0].STRCMP("REPAIR"))
		{
			return RunRepairWorkOrder(workOrder,args);
		}
		if(YSOK==RunCustomWorkOrder(workOrder,args))
		{
			return YSOK;
		}

		YsString errorReason;
		errorReason.Printf("Unrecognized primary command [%s]",args[0].Txt());
		ShowError(workOrder,errorReason);
		return YSERR;
	}
	return YSOK; // Empty work order.  Nothing to do.
}

/* virtual */void GeblCmd_WorkOrder::ShowError(const YsString &workOrder,const YsString &reason) const
{
	fprintf(stderr,"Error in work order!\n");
	fprintf(stderr,"Work Order: %s\n",workOrder.Txt());
	fprintf(stderr,"Reason: %s\n",reason.Txt());
}

void GeblCmd_WorkOrder::ShowHelp(void) const
{
	GeblCmd_CommandParameterInfo::ShowHelp();

	printf("<<Work-Order Strings>>\n");
	printf("    rem                                Comment line.\n");
	printf("    showhelp                           Show this help.\n");

	printf("  Configuration commands\n");
	printf("    config tolerance 0.000001          Set tolerance to 0.000001\n");
	printf("    config tolerance auto              Set tolerance to the diagonal length of the input model divided by 10e-8.\n");
	printf("                                       (Therefore must be used after opening an input file.)\n");
	printf("    config colortolerance 3            Set color tolerance to 3 (0-255 scale).  Default value is 3.\n");
	printf("    config numthread 4                 Set number of threads to 4.\n");

	printf("  Orientation commands\n");
	printf("    orientation normal_from_orientation  Re-calculate normal vectors assuming that the polygons are oriented correctly.\n");
	printf("    orientation recalc_as_closed_solid   Correct polygon orientation assuming that the model is a closed solid.\n");
	printf("                                         Also re-calculate normal vectors.\n");
	printf("    orientation expand_normal            Expand normal vectors to the ones that do not have an assigned normal.\n");

	printf("  Constraint-Edge Commands\n");
	printf("    constedge clear                    Clear all const edges.\n");
	printf("    constedge add fgboundary           Add const edges along the face-group boundaries.\n");
	printf("    constedge add dha 60               Add const edges where dihedral angle exceeds 60 degrees.\n");
	printf("    constedge add nonmanifold          Add const edges along non-manifold edges.\n");
	printf("    constedge reconsider               Split const edges at const-edge intersections and merge const-edges\n");
	printf("                                       where only two const-edges are meeting.\n");
	printf("    constedge add all                  Make all edge pieces a const edge.\n");
	printf("    constedge makenobubble all         Make all no-bubble edge.\n");
	// constedge add allinfgid_nobubble fgId      Add const edges on all edges within the face group and set no-bubble flag.
	// constedge add allinfgname_nobubble fgName  Add const edges on all edges within the face group and set no-bubble flag.
	printf("    constedge delete short 12.3       Delete const edges shorter than the given threshold.\n");
	printf("    constedge delete short_isolated 12.3\n");
	printf("                                      Delete const edges that are not connected to other const edges\n");
	printf("                                      and shorter than the given threshold.\n");
	printf("    constedge delete dhahigh 170      Delete const edges on dihedral angle higher than 170 degrees.\n");

	printf("  Face-Group Commands.\n");
	printf("    facegroup create fromce            Create face groups based on the constraint edges.\n");
	printf("    facegroup elemsize name \"name\" size\n");
	printf("                       ident ident# size\n");
	printf("                       color rrggbb size\n");
	printf("                       all size\n");
	printf("                       nearpos x y z size\n");
	printf("                                       Specify desired element size (edge length) on face group.\n");
	printf("                                       Can specify multiple in one command by repeating name/ident# and size.\n");
	printf("                                       RRGGBB is a color coded in the hexa-decimal number.\n");
	printf("                                       A name is case sensitive.\n");
	printf("    facegroup blayer name \"name\" #ofLayers 1stLayerThickness funcType funcParam\n");
	printf("                     ident ident# #ofLayers 1stLayerThickness funcType funcParam\n");
	printf("                     color rrggbb #ofLayers 1stLayerThickness funcType funcParam\n");
	printf("                     all #ofLayers 1stLayerThickness funcType funcParam\n");
	printf("                     nearpos x y z #ofLayers 1stLayerThickness funcType funcParam\n");
	printf("                                       Specify boundary-layer specification on face groups.\n");
	printf("                                       Can specify multiple in one command by repeating name/ident# through funcParam.\n");
	printf("                                       RRGGBB is a color coded in the hexa-decimal number.\n");
	printf("                                       A name is case sensitive.\n");
	printf("    facegroup clearblayer name \"name\"\n");
	printf("                     ident ident#\n");
	printf("                     all\n");
	printf("                     nearpos x y z\n");
	printf("                                       Remove boundary-layer specification from face groups.\n");
	printf("                                       Can specify multiple in one command by repeating name/ident#.\n");
	printf("                                       A name is case sensitive.\n");
	printf("    facegroup makenobubble all         Make all face-groups no-bubble.\n");
	printf("    facegroup extract name \"name\"      Extract face group, or delete any vertices, const-edges, and polygons that\n");
	printf("                      ident ident#     does not belong to the given face groups.  Can specify multiple face groups.\n");
	printf("                      color rrggbb     Also deletes other face groups.\n");
	printf("                      nearpos x y z\n");

	printf("  Merge-Vertexes Commands\n");
	printf("    mergevertex all 0.00001            Merge all vertices within tolerance of 0.00001\n");
	printf("    mergevertex nonmanifold 0.00001    Merge vertices on non-manifold edges with tolerance of 0.00001\n");
	printf("         If tolerance is not given, the default tolerance will be used.\n");

	printf("  Global Commands\n");
	printf("    global delete_unused_vertex        Delete all unused vertices.\n");

	printf("  Stitching Commands\n");
	printf("    stitch free                        Free stitching.\n");
	printf("    stitch loop                        Loop stitching.\n");

	printf("  Repair Commands\n");
	printf("    repair delete identical_polygon    Delete identical polygons.\n");
	printf("    repair delete flat_triangle [cannot_calc_normal|narrower_than_tolerance] [collapse_tol|swap|merge] [collapse_tol|swap|merge] ...\n");
	printf("                                       Try to kill flat triangles.  The first argument to the command is a criterion for\n");
	printf("                                       identifying flat triangles.  The second argument and on are the schemes for killing\n");
	printf("                                       flat triangles.  The schemes will be applied in the order given in the arguments.\n");

	printf("  File Input/Output\n");
	printf("    fileio open filename               Open a file.  File type is identified based on the extension.\n");
	printf("    fileio opensrf filename            Open .SRF file.\n");
	printf("    fileio save filename               Save file.  (Works independent of -in and -out)\n");
	printf("    fileio savevertexcurvature filename      Save curvatures at vertices.\n");

	printf("  Control\n");
	printf("    control pause                      Pause the execution (wait for ENTER)\n");

	ShowCustomHelp();
}

/* virtual */ YSRESULT GeblCmd_WorkOrder::RunCustomWorkOrder(const YsString &,const YsArray <YsString,16> &)
{
	return YSERR;
}

/* virtual */ void GeblCmd_WorkOrder::ShowCustomHelp(void) const
{
}
