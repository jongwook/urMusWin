cloudsbackdropregion=Region('region', 'cloudsbackdropregion', UIParent);
cloudsbackdropregion:SetWidth(ScreenWidth());
cloudsbackdropregion:SetHeight(ScreenHeight());
cloudsbackdropregion:SetLayer("BACKGROUND");
cloudsbackdropregion:SetAnchor('BOTTOMLEFT',0,0); 
--cloudsbackdropregion:EnableClamping(true)
cloudsbackdropregion.texture = cloudsbackdropregion:Texture("cloud_sequencer.png");
cloudsbackdropregion.texture:SetGradientColor("TOP",255,255,255,255,255,255,255,255);
cloudsbackdropregion.texture:SetGradientColor("BOTTOM",255,255,255,255,255,255,255,255);
--cloudsbackdropregion.texture:SetBlendMode("BLEND")
cloudsbackdropregion.texture:SetTexCoord(0,0.63,0.94,0.0);
--cloudsbackdropregion:EnableInput(true);
cloudsbackdropregion:Show();


function ShowPopup(note)
	popuptextregion.textlabel:SetLabel(note)
	popuptextregion.staytime = 1.5
	popuptextregion.fadetime = 2.0
	popuptextregion.alpha = 1
	popuptextregion.alphaslope = 2
	popuptextregion:Handle("OnUpdate", FadePopup)
	popuptextregion:SetAlpha(1.0)
	popuptextregion:Show()
end

popuptextregion=Region('region', 'popuptextregion', UIParent)
popuptextregion:SetWidth(ScreenWidth())
popuptextregion:SetHeight(48*2)
popuptextregion:SetLayer("TOOLTIP")
popuptextregion:SetAnchor('BOTTOMLEFT',0,ScreenHeight()/2-24) 
popuptextregion:EnableClamping(true)
popuptextregion:Show()
popuptextregion.textlabel=popuptextregion:TextLabel()
popuptextregion.textlabel:SetFont("arial.ttf")
popuptextregion.textlabel:SetHorizontalAlign("CENTER")
popuptextregion.textlabel:SetLabelHeight(48)
popuptextregion.textlabel:SetColor(0,0,60,190)
popuptextregion.textlabel:SetShadowColor(0,0,0,190)
popuptextregion.textlabel:SetShadowOffset(4,-6)
popuptextregion.textlabel:SetShadowBlur(6.0)


