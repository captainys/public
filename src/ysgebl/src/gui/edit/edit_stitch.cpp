#include "../ysgebl_gui_editor_base.h"
#include "../textresource.h"



void GeblGuiEditorBase::Edit_SimpleStitch(FsGuiPopUpMenuItem *)
{
    if(nullptr!=slHd)
    {
        YsShellExtEdit::StopIncUndo undoGuard(*slHd);

        auto &shl=*slHd;
        auto selVtHd=shl.GetSelectedVertex();
        if(3<selVtHd.size())
        {
            MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST3VERTEX);
            return;
        }

        auto usingPlHd=shl.FindPolygonFromEdgePiece(selVtHd[0],selVtHd.back());
        auto usingCeHd=shl.FindConstEdgeFromEdgePiece(selVtHd[0],selVtHd.back());
        if(0==usingPlHd.size() && 0==usingCeHd.size())
        {
            MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NOPLCE_CONNECT_FIRST_LAST_VTX);
            return;
        }

        for(auto plHd : usingPlHd)
        {
            auto plVtHd=shl.GetPolygonVertex(plHd);
            decltype(plVtHd) newPlVtHd;
            for(YSSIZE_T idx=0; idx<plVtHd.size(); ++idx)
            {
                newPlVtHd.Add(plVtHd[idx]);
                if(plVtHd[idx]==selVtHd[0] && plVtHd.GetCyclic(idx+1)==selVtHd.Last())
                {
                    for(YSSIZE_T j=1; j<selVtHd.GetN()-1; ++j)
                    {
                        newPlVtHd.Add(selVtHd[j]);
                    }
                }
                else if(plVtHd[idx]==selVtHd.Last() && plVtHd.GetCyclic(idx+1)==selVtHd[0])
                {
                    for(YSSIZE_T j=selVtHd.GetN()-2; 0<j; --j)
                    {
                        newPlVtHd.Add(selVtHd[j]);
                    }
                }
            }
            shl.SetPolygonVertex(plHd,newPlVtHd);
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
            SetNeedRedraw(YSTRUE);
        }
        for(auto ceHd : usingCeHd)
        {
            auto ceVtHd=shl.GetConstEdgeVertex(ceHd);
            auto isLoop=shl.IsConstEdgeLoop(ceHd);

            YsArray <YsShell::VertexHandle> newCeVtHd;
            YSSIZE_T last=(YSTRUE==isLoop ? ceVtHd.GetN() : ceVtHd.GetN()-1);
            for(YSSIZE_T idx=0; idx<last; ++idx)
            {
                newCeVtHd.Add(ceVtHd[idx]);
                if(ceVtHd[idx]==selVtHd[0] && ceVtHd.GetCyclic(idx+1)==selVtHd.Last())
                {
                    for(YSSIZE_T j=1; j<selVtHd.GetN()-1; ++j)
                    {
                        newCeVtHd.Add(selVtHd[j]);
                    }
                }
                else if(ceVtHd[idx]==selVtHd.Last() && ceVtHd.GetCyclic(idx+1)==selVtHd[0])
                {
                    for(YSSIZE_T j=selVtHd.GetN()-2; 0<j; --j)
                    {
                        newCeVtHd.Add(selVtHd[j]);
                    }
                }
            }
            if(YSTRUE!=isLoop)
            {
                newCeVtHd.push_back(ceVtHd.back());
            }
            shl.ModifyConstEdge(ceHd,newCeVtHd,isLoop);
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_CONSTEDGE;
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
            SetNeedRedraw(YSTRUE);
        }
    }    
}
