
local fingerposx, fingerposy = InputPosition()

function Paint(self)
	local x,y = InputPosition()
	if x ~= fingerposx or y ~= fingerposy then
		self.texture:Line(fingerposx, fingerposy, x, y)
--		self.texture:Point(x,y)
		fingerposx, fingerposy = x,y
	end
end

function Clear(self)
	smudgebackdropregion.texture:Clear(0.8,0.8,0.8);
end

smudgebackdropregion=Region('region', 'smudgebackdropregion', UIParent);
smudgebackdropregion:SetWidth(ScreenWidth());
smudgebackdropregion:SetHeight(ScreenHeight());
smudgebackdropregion:SetLayer("BACKGROUND");
smudgebackdropregion:SetAnchor('BOTTOMLEFT',0,0); 
--smudgebackdropregion:EnableClamping(true)
smudgebackdropregion.texture = smudgebackdropregion:Texture("Default.png");
smudgebackdropregion.texture:SetGradientColor("TOP",255,255,255,255,255,255,255,255);
smudgebackdropregion.texture:SetGradientColor("BOTTOM",255,255,255,255,255,255,255,255);
--smudgebackdropregion.texture:SetBlendMode("BLEND")
smudgebackdropregion.texture:SetTexCoord(0,0.63,0.94,0.0);
smudgebackdropregion:Handle("OnUpdate", Paint);
smudgebackdropregion:Handle("OnDoubleTap", Clear);
smudgebackdropregion:EnableInput(true);
smudgebackdropregion:Show();
--smudgebackdropregion.texture:Clear(0.8,0.8,0.8);

brush1=Region('region','brush',UIParent)
brush1.t=brush1:Texture()
brush1.t:SetTexture("circlebutton-16.png");
brush1:UseAsBrush();
--brush1.t:SetBrushSize(1);

--[[function FlipPage(self)
--	SetPage(1)
--	return
	if not playgroundloaded then
		SetPage(13)
		dofile(SystemPath("urPlayground.lua"))
		playgroundloaded = true
	else
		SetPage(13);
	end
end--]]

local pagebutton=Region('region', 'pagebutton', UIParent);
pagebutton:SetWidth(pagersize);
pagebutton:SetHeight(pagersize);
pagebutton:SetLayer("TOOLTIP");
pagebutton:SetAnchor('BOTTOMLEFT',ScreenWidth()-pagersize-4,ScreenHeight()-pagersize-4); 
pagebutton:EnableClamping(true)
--pagebutton:Handle("OnDoubleTap", FlipPage)
pagebutton:Handle("OnTouchDown", FlipPage)
pagebutton.texture = pagebutton:Texture("circlebutton-16.png");
pagebutton.texture:SetGradientColor("TOP",255,255,255,255,255,255,255,255);
pagebutton.texture:SetGradientColor("BOTTOM",255,255,255,255,255,255,255,255);
pagebutton.texture:SetBlendMode("BLEND")
pagebutton.texture:SetTexCoord(0,1.0,0,1.0);
pagebutton:EnableInput(true);
pagebutton:Show();
