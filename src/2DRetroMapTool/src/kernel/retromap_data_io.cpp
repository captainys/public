#include "retromap_data.h"


void RetroMap_World::ConfigureTextureNoSaveFlag(void)
{
	for(auto texHd : texMan.AllTexture())
	{
		texMan.SetDontSave(texHd,YSTRUE);
	}
	for(auto fdHd : AllField())
	{
		for(auto mpHd : AllMapPiece(fdHd))
		{
			auto mapPiecePtr=GetMapPiece(fdHd,mpHd);
			texMan.SetDontSave(mapPiecePtr->texHd,YSFALSE);
		}
	}
}

void RetroMap_World::MarkUp::ArrowHeadInfo::Write(YsTextOutputStream &outStream,const char typeStr[]) const
{
	switch(arrowHeadType)
	{
	default:
	case MarkUp::ARROWHEAD_NONE:
		outStream.Printf("%s AHTYPE NONE\n",typeStr);
		break;
	case MarkUp::ARROWHEAD_TRIANGLE:	
		outStream.Printf("%s AHTYPE TRI\n",typeStr);
		break;
	}
	outStream.Printf("%s AHSIZE %lf\n",typeStr,arrowHeadSize);
}

void RetroMap_World::MarkUp::ArrowHeadInfo::Read(const YsConstArrayMask <YsString> &argv)
{
	if(2<=argv.size())
	{
		if(0==argv[1].STRCMP("AHTYPE") && 3<=argv.size())
		{
			if(0==argv[2].STRCMP("NONE"))
			{
				arrowHeadType=MarkUp::ARROWHEAD_NONE;
			}
			else if(0==argv[2].STRCMP("TRI"))
			{
				arrowHeadType=MarkUp::ARROWHEAD_TRIANGLE;
			}
		}
		else if(0==argv[1].STRCMP("AHSIZE") && 3<=argv.size())
		{
			arrowHeadSize=atof(argv[2]);
		}
	}
}

YSRESULT RetroMap_World::Save(FILE *fp) const
{
	YsTextFileOutputStream outStream(fp);
	outStream.Printf("BEGINRETRORPGMAP\n");
	outStream.Printf("VERSION 1\n");

	outStream.Printf("# Texture Manager\n");

	texMan.Save(outStream);

	outStream.Printf("# End of Texture Manager\n");

	outStream.Printf("GAMETITLE \"%s\"\n",gameTitle.GetUTF8String().Txt());
	outStream.Printf("LASTIMAGE \"%s\"\n",lastImageFileName.GetUTF8String().Txt());

	if(0<trimRect.GetWidth() && 0<trimRect.GetHeight())
	{
		outStream.Printf("TRIMRECT %d %d %d %d\n",trimRect.Min().x(),trimRect.Min().y(),trimRect.Max().x(),trimRect.Max().y());
	}
	outStream.Printf("DEFSCALING %.8lf %.8lf\n",defScaling.x(),defScaling.y());
	outStream.Printf("UNIT %d %d\n",unit.x(),unit.y());
	outStream.Printf("DEFARWSIZE %lf\n",defArrowSize);
	outStream.Printf("DIFFTHR %d\n",(int)diffThr);
	outStream.Printf("DEFFONTSIZE %d\n",(int)defFontSize);
	outStream.Printf("DEFMARKUPFGCOL %d %d %d %d\n",defMarkUpFgCol.Ri(),defMarkUpFgCol.Gi(),defMarkUpFgCol.Bi(),defMarkUpFgCol.Ai());
	outStream.Printf("DEFMARKUPBGCOL %d %d %d %d\n",defMarkUpBgCol.Ri(),defMarkUpBgCol.Gi(),defMarkUpBgCol.Bi(),defMarkUpBgCol.Ai());

	for(auto &field : allField)
	{
		field.mapPiece.Encache();
		field.markUp.Encache();
		field.anchor.Encache();

		outStream.Printf("BEGINFIELD\n");
		outStream.Printf("FIELDNAME \"%s\"\n",field.fieldName.GetUTF8String().Txt());
		for(auto &mapPiece : field.mapPiece)
		{
			outStream.Printf("BEGINMAPPIECE\n");
			auto unitPtr=texMan.GetTexture(mapPiece.texHd);
			outStream.Printf("TEXLABEL \"%s\"\n",unitPtr->GetLabel().Txt());
			outStream.Printf("POS %d %d\n",mapPiece.shape.pos.x(),mapPiece.shape.pos.y());
			outStream.Printf("SCALING %.8lf %.8lf\n",mapPiece.shape.scaling.x(),mapPiece.shape.scaling.y());
			outStream.Printf("TRIMRECT %d %d %d %d\n",
			    mapPiece.shape.visibleArea.Min().x(),mapPiece.shape.visibleArea.Min().y(),
			    mapPiece.shape.visibleArea.Max().x(),mapPiece.shape.visibleArea.Max().y());
			outStream.Printf("EVENTLEVEL %d\n",mapPiece.eventLevel);
			outStream.Printf("ALPHA %lf\n",mapPiece.alpha);
			outStream.Printf("ENDMAPPIECE\n");
		}

		for(auto &markUp : field.markUp)
		{
			outStream.Printf("BEGINMARKUP\n");
			outStream.Printf("FGCOL %d %d %d %d\n",markUp.fgCol.Ri(),markUp.fgCol.Gi(),markUp.fgCol.Bi(),markUp.fgCol.Ai());
			outStream.Printf("BGCOL %d %d %d %d\n",markUp.bgCol.Ri(),markUp.bgCol.Gi(),markUp.bgCol.Bi(),markUp.bgCol.Ai());
			outStream.Printf("LINE %s\n",YsBoolToStr(markUp.line));
			outStream.Printf("FILL %s\n",YsBoolToStr(markUp.fill));
			outStream.Printf("LOOP %s\n",YsBoolToStr(markUp.closed));
			switch(markUp.markUpType)
			{
			case MarkUp::MARKUP_STRING:
				{
					auto utf8=markUp.txt.GetUTF8String();
					for(int i=0; i<utf8.Strlen(); ++i)
					{
						if(utf8[i]=='\"')
						{
							utf8.Set(i,'\'');
						}
					}
					outStream.Printf("STRING \"%s\"\n",utf8.Txt());
					outStream.Printf("FONTSIZE %d\n",(int)markUp.fontSize);
					outStream.Printf("POS %d %d\n",markUp.pos.x(),markUp.pos.y());
				}
				break;
			case MarkUp::MARKUP_POINT_ARRAY:
				outStream.Printf("POINTARRAY");
				for(auto p : markUp.plg)
				{
					outStream.Printf(" %d %d",p.x(),p.y());
				}
				outStream.Printf("\n");
				markUp.frontArrow.Write(outStream,"FRTARW");
				markUp.backArrow.Write(outStream,"BCKARW");
				break;
			}
			outStream.Printf("ENDMARKUP\n");
		}

		for(auto &a : field.anchor)
		{
			auto muHd=field.FindMarkUp(a.markUpKey);
			auto baseMpHd=field.FindMapPiece(a.baseMapPieceKey);
			auto baseMuHd=field.FindMarkUp(a.baseMarkUpKey);
			if(nullptr!=muHd && (nullptr!=baseMpHd || nullptr!=baseMuHd))
			{
				auto muIdx=field.markUp.GetIndexFromHandle(muHd);
				outStream.Printf("BEGINANCHOR\n");
				outStream.Printf("MARKUP %d\n",(int)muIdx);
				if(nullptr!=baseMpHd)
				{
					auto mpIdx=field.mapPiece.GetIndexFromHandle(baseMpHd);
					outStream.Printf("BASEMAPPIECE %d\n",(int)mpIdx);
				}
				if(nullptr!=baseMuHd)
				{
					auto muIdx=field.markUp.GetIndexFromHandle(baseMuHd);
					outStream.Printf("BASEMARKUP %d\n",(int)muIdx);
				}
				outStream.Printf("POINTINDEX %d\n",(int)a.markUpPointIdx);
				outStream.Printf("RELPOS %d %d\n",(int)a.relPosToBase.x(),(int)a.relPosToBase.y());
				outStream.Printf("ENDANCHOR\n");
			}
		}

		outStream.Printf("ENDFIELD\n");
	}

	outStream.Printf("ENDRETRORPGMAP\n");

	modified=YSFALSE;
	lastSavedUndoPtr=undoPtr;

	return YSOK;
}

YSRESULT RetroMap_World::Open(FILE *fp)
{
	CleanUp();
	
	enum STATE
	{
		STATE_NORMAL,
		STATE_FIELD,
		STATE_MAPPIECE,
		STATE_MARKUP,
		STATE_ANCHOR,
	};

	YsArray <STATE> stateStack;
	YsArray <TentativeAnchor> tentativeAnchor;
	stateStack.Add(STATE_NORMAL);
	FieldHandle fdHd=nullptr;
	MapPieceHandle mpHd=nullptr;
	MarkUpHandle muHd=nullptr;
	texMan.BeginLoadTexMan();

	YsString str;
	while(nullptr!=str.Fgets(fp) && 0<stateStack.GetN())
	{
		auto argv=str.Argv();
		for(YSSIZE_T i=0; i<argv.GetN(); ++i)
		{
			if(argv[i][0]=='#')
			{
				argv.Resize(i);
				break;
			}
		}

		switch(stateStack.Last())
		{
		case STATE_NORMAL:
			if(0<argv.GetN())
			{
				if(0==argv[0].STRCMP("BEGINFIELD"))
				{
					stateStack.Add(STATE_FIELD);
					fdHd=allField.Create();
					allField[fdHd]->CleanUp();
					allField[fdHd]->owner=this;
					tentativeAnchor.CleanUp();
				}
				else if(0==argv[0].STRCMP("GAMETITLE") && 2<=argv.GetN())
				{
					gameTitle.SetUTF8String(argv[1]);
				}
				else if(0==argv[0].STRCMP("LASTIMAGE") && 2<=argv.GetN())
				{
					lastImageFileName.SetUTF8String(argv[1]);
				}
				else if(0==argv[0].STRCMP("TRIMRECT") && 5<=argv.GetN())
				{
					auto x0=atoi(argv[1]);
					auto y0=atoi(argv[2]);
					auto x1=atoi(argv[3]);
					auto y1=atoi(argv[4]);
					trimRect.Set(YsVec2i(x0,y0),YsVec2i(x1,y1));
				}
				else if(0==argv[0].STRCMP("DEFSCALING") && 3<=argv.GetN())
				{
					defScaling.Set(atof(argv[1]),atof(argv[2]));
				}
				else if(0==argv[0].STRCMP("UNIT") && 3<=argv.GetN())
				{
					auto x0=atoi(argv[1]);
					auto y0=atoi(argv[2]);
					unit.Set(x0,y0);
				}
				else if(0==argv[0].STRCMP("DEFARWSIZE") && 2<=argv.size())
				{
					this->defArrowSize=atof(argv[1]);
				}
				else if(0==argv[0].STRCMP("DIFFTHR") && 2<=argv.GetN())
				{
					this->diffThr=atoi(argv[1]);
				}
				else if(0==argv[0].STRCMP("DEFFONTSIZE") && 2<=argv.GetN())
				{
					defFontSize=YsGreater(atoi(argv[1]),8);
				}
				else if(0==argv[0].STRCMP("DEFMARKUPFGCOL") && 5<=argv.GetN())
				{
					int r=atoi(argv[1]);
					int g=atoi(argv[2]);
					int b=atoi(argv[3]);
					int a=atoi(argv[4]);
					defMarkUpFgCol.SetIntRGBA(r,g,b,a);
				}
				else if(0==argv[0].STRCMP("DEFMARKUPBGCOL") && 5<=argv.GetN())
				{
					int r=atoi(argv[1]);
					int g=atoi(argv[2]);
					int b=atoi(argv[3]);
					int a=atoi(argv[4]);
					defMarkUpBgCol.SetIntRGBA(r,g,b,a);
				}
				else if(0==argv[0].STRCMP("BEGINRETRORPGMAP"))
				{
				}
				else if(0==argv[0].STRCMP("ENDRETRORPGMAP"))
				{
					stateStack.DeleteLast();
				}
				else if(0==argv[0].STRCMP("VERSION"))
				{
				}
				else if(0==argv[0].STRCMP("TEXMAN") && 2<=argv.GetN())
				{
					if(YSOK!=texMan.LoadTexManOneLine(str,argv.GetN(),argv))
					{
						return YSERR;
					}
				}
			}
			break;
		case STATE_FIELD:
			if(0<argv.GetN())
			{
				if(0==argv[0].STRCMP("ENDFIELD"))
				{
					stateStack.DeleteLast();

					auto fieldPtr=allField[fdHd];
					fieldPtr->mapPiece.Encache();
					fieldPtr->markUp.Encache();
					for(auto t : tentativeAnchor)
					{
						auto baseMpHd=fieldPtr->mapPiece.GetObjectHandleFromIndex(t.baseMapPieceIdx);
						auto baseMuHd=fieldPtr->markUp.GetObjectHandleFromIndex(t.baseMarkUpIdx);
						auto muHd=fieldPtr->markUp.GetObjectHandleFromIndex(t.markUpIdx);
						if(nullptr!=muHd)
						{
							auto muPtr=fieldPtr->GetMarkUp(muHd);
							if(nullptr!=muPtr && YSTRUE==muPtr->IsDrawing())
							{
								if(nullptr!=baseMpHd)
								{
									fieldPtr->CreateAnchor(muHd,t.markUpPointIdx,baseMpHd,t.relPosToBase);
								}
								if(nullptr!=baseMuHd)
								{
									fieldPtr->CreateAnchor(muHd,t.markUpPointIdx,baseMuHd,t.relPosToBase);
								}
							}
						}
					}
				}
				else
				{
					auto fieldPtr=allField[fdHd];
					if(nullptr!=fieldPtr)
					{
						if(0==argv[0].STRCMP("FIELDNAME") && 2<=argv.GetN())
						{
							fieldPtr->fieldName.SetUTF8String(argv[1]);
						}
						else if(0==argv[0].STRCMP("BEGINMAPPIECE"))
						{
							mpHd=fieldPtr->CreateMapPiece();
							stateStack.Add(STATE_MAPPIECE);
						}
						else if(0==argv[0].STRCMP("BEGINMARKUP"))
						{
							muHd=fieldPtr->CreateMarkUp();
							stateStack.Add(STATE_MARKUP);
						}
						else if(0==argv[0].STRCMP("BEGINANCHOR"))
						{
							tentativeAnchor.Increment();
							tentativeAnchor.back().Initialize();
							stateStack.Add(STATE_ANCHOR);
						}
					}
				}
			}
			break;
		case STATE_MAPPIECE:
			if(0<argv.GetN())
			{
				if(0==argv[0].STRCMP("ENDMAPPIECE"))
				{
					stateStack.DeleteLast();
				}
				else
				{
					auto fieldPtr=allField[fdHd];
					if(nullptr!=fieldPtr)
					{
						auto mapPiecePtr=fieldPtr->mapPiece[mpHd];
						if(nullptr!=mapPiecePtr)
						{
							if(0==argv[0].STRCMP("TEXLABEL") && 2<=argv.GetN())
							{
								mapPiecePtr->texLabelCache=argv[1];
							}
							else if(0==argv[0].STRCMP("POS") && 3<=argv.GetN())
							{
								mapPiecePtr->shape.pos.Set(atoi(argv[1]),atoi(argv[2]));
							}
							else if(0==argv[0].STRCMP("SCALING") && 3<=argv.GetN())
							{
								mapPiecePtr->shape.scaling.Set(atof(argv[1]),atof(argv[2]));
							}
							else if(0==argv[0].STRCMP("TRIMRECT") && 5<=argv.GetN())
							{
								auto x0=atoi(argv[1]);
								auto y0=atoi(argv[2]);
								auto x1=atoi(argv[3]);
								auto y1=atoi(argv[4]);
								mapPiecePtr->shape.visibleArea.Set(YsVec2i(x0,y0),YsVec2i(x1,y1));
							}
							else if(0==argv[0].STRCMP("EVENTLEVEL") && 2<=argv.GetN())
							{
								mapPiecePtr->eventLevel=atoi(argv[1]);
							}
							else if(0==argv[0].STRCMP("ALPHA") && 2<=argv.GetN())
							{
								mapPiecePtr->alpha=atof(argv[1]);
							}
						}
					}
				}
			}
			break;
		case STATE_MARKUP:
			if(0<argv.GetN())
			{
				if(0==argv[0].STRCMP("ENDMARKUP"))
				{
					RemakeStringTexture(fdHd,muHd);
					ReadyTexture(fdHd,muHd);
					ReadyVbo(fdHd,muHd);
					stateStack.DeleteLast();
				}
				else
				{
					auto fieldPtr=allField[fdHd];
					if(nullptr!=fieldPtr)
					{
						auto markUpPtr=fieldPtr->markUp[muHd];
						if(0==argv[0].STRCMP("POS") && 3<=argv.GetN())
						{
							markUpPtr->pos.Set(atoi(argv[1]),atoi(argv[2]));
						}
						else if(0==argv[0].STRCMP("FGCOL") && 5<=argv.GetN())
						{
							int r=atoi(argv[1]);
							int g=atoi(argv[2]);
							int b=atoi(argv[3]);
							int a=atoi(argv[4]);
							markUpPtr->fgCol.SetIntRGBA(r,g,b,a);
						}
						else if(0==argv[0].STRCMP("BGCOL") && 5<=argv.GetN())
						{
							int r=atoi(argv[1]);
							int g=atoi(argv[2]);
							int b=atoi(argv[3]);
							int a=atoi(argv[4]);
							markUpPtr->bgCol.SetIntRGBA(r,g,b,a);
						}
						else if(0==argv[0].STRCMP("STRING") && 2<=argv.GetN())
						{
							markUpPtr->MakeStringMarkUp(argv[1]);
						}
						else if(0==argv[0].STRCMP("FONTSIZE") && 2<=argv.GetN())
						{
							markUpPtr->fontSize=atoi(argv[1]);
						}
						else if(0==argv[0].STRCMP("POINTARRAY"))
						{
							YsArray <YsVec2i> plg;
							for(YSSIZE_T i=1; i<=argv.GetN()-2; i+=2)
							{
								plg.Add(YsVec2i(atoi(argv[i]),atoi(argv[i+1])));
							}
							markUpPtr->MakePointArrayMarkUp(plg);
						}
						else if(0==argv[0].STRCMP("LINE") && 2<=argv.GetN())
						{
							markUpPtr->line=YsStrToBool(argv[1]);
						}
						else if(0==argv[0].STRCMP("FILL") && 2<=argv.GetN())
						{
							markUpPtr->fill=YsStrToBool(argv[1]);
						}
						else if(0==argv[0].STRCMP("LOOP") && 2<=argv.GetN())
						{
							markUpPtr->closed=YsStrToBool(argv[1]);
						}
						else if(0==argv[0].STRCMP("FRTARW"))
						{
							markUpPtr->frontArrow.Read(argv);
						}
						else if(0==argv[0].STRCMP("BCKARW"))
						{
							markUpPtr->backArrow.Read(argv);
						}
					}
				}
			}
			break;
		case STATE_ANCHOR:
			if(0<argv.GetN())
			{
				if(0==argv[0].STRCMP("ENDANCHOR"))
				{
					stateStack.DeleteLast();
				}
				else if(0==argv[0].STRCMP("MARKUP") && 2<=argv.size())
				{
					tentativeAnchor.back().markUpIdx=atoi(argv[1]);
				}
				else if(0==argv[0].STRCMP("BASEMAPPIECE") && 2<=argv.size())
				{
					tentativeAnchor.back().baseMapPieceIdx=atoi(argv[1]);
				}
				else if(0==argv[0].STRCMP("BASEMARKUP") && 2<=argv.size())
				{
					tentativeAnchor.back().baseMarkUpIdx=atoi(argv[1]);
				}
				else if(0==argv[0].STRCMP("POINTINDEX") && 2<=argv.size())
				{
					tentativeAnchor.back().markUpPointIdx=atoi(argv[1]);
				}
				else if(0==argv[0].STRCMP("RELPOS") && 3<=argv.size())
				{
					tentativeAnchor.back().relPosToBase.Set(atoi(argv[1]),atoi(argv[2]));
				}
			}
			break;
		}
	}
	texMan.EndLoadTexMan();

	for(auto &field : allField)
	{
		for(auto &mapPiece : field.mapPiece)
		{
			mapPiece.texHd=texMan.FindTextureFromLabel(mapPiece.texLabelCache);
			if(nullptr==mapPiece.texHd)
			{
				printf("Texture label %s not found!\n",mapPiece.texLabelCache.Txt());
			}

			// Testing delayed-texture expansion.
			// mapPiece.ReadyTexture();
			// mapPiece.ReadyVbo();

			// auto verify=mapPiece.GetBitmapSize();
			// printf("%d %d\n",verify.x(),verify.y());
			// printf("%d %d\n",mapPiece.shape.pos.x(),mapPiece.shape.pos.y());
			auto trimRect=mapPiece.shape.visibleArea;
			printf("%d %d %d %d\n",trimRect.Min().x(),trimRect.Min().y(),trimRect.Max().x(),trimRect.Max().y());
		}
	}

	return YSOK;
}
