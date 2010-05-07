-- urPong, homage to the beginning of it all... again
-- Hack by Bryan Summersett
-- Minor tweaks by Georg Essl
-- Created 2/11/2010

local function Shutdown()
	dac:RemovePullLink(0, upSample, 0)
	dac:RemovePullLink(0, upSample2, 0)
end

local function ReInit(self)
	dac:SetPullLink(0, upSample, 0)
	dac:SetPullLink(0, upSample2, 0)
end

-- Instantiating our pong background
pongBGRegion = Region('Region', 'pongBGRegion', UIParent)
pongBGRegion:SetWidth(ScreenWidth())
pongBGRegion:SetHeight(ScreenHeight())
pongBGRegion:SetLayer("BACKGROUND")
pongBGRegion:SetAnchor('BOTTOMLEFT',0,0) -- why here?
pongBGRegion.texture = pongBGRegion:Texture("PongBG.png")
pongBGRegion.texture:SetTexCoord(0,0.63,0.94,0.0)
pongBGRegion:Show()
pongBGRegion:Handle("OnPageEntered", ReInit)
pongBGRegion:Handle("OnPageLeft", Shutdown)

MAXSCORE = 10
DEFAULT_VELOCITY = 3

-- Notification Information Overlay

function FadePopup(self, elapsed)
	if self.staytime > 0 then
		self.staytime = self.staytime - elapsed
		return
	end
	if self.fadetime > 0 then
		self.fadetime = self.fadetime - elapsed
		self.alpha = self.alpha - self.alphaslope * elapsed
		self:SetAlpha(self.alpha)
	else
		self:Hide()
		self:Handle("OnUpdate", nil)
	end
end

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
popuptextregion.textlabel:SetFont("Trebuchet MS")
popuptextregion.textlabel:SetHorizontalAlign("CENTER")
popuptextregion.textlabel:SetLabelHeight(48)
popuptextregion.textlabel:SetColor(0,0,60,190)
popuptextregion.textlabel:SetShadowColor(0,0,0,190)
popuptextregion.textlabel:SetShadowOffset(4,-6)
popuptextregion.textlabel:SetShadowBlur(6.0)

ShowPopup("urPong!")

function FlashNumber(self,elapsed)
	self.flashtime = self.flashtime + elapsed
	if self.flashtime > self.holdtime then
		self.flashtime = 0
		self:Handle("OnUpdate",nil)
		self.textlabel:SetColor(255,0,0,255)
	end
end

function MakeScoreLabel(x, y)
    local score=Region('Region', "score"..x, UIParent)
    score:SetWidth(ScreenWidth()/3)
    score:SetHeight(44)
    score:SetLayer("TOOLTIP")
    score:SetAnchor("BOTTOMLEFT", x-ScreenWidth()/3/2, y)
    score.textlabel=score:TextLabel()
    score.textlabel:SetFont("Trebuchet MS")
    score.textlabel:SetHorizontalAlign("CENTER")
    score.textlabel:SetVerticalAlign("TOP")
    score.textlabel:SetLabel("0")
    score.textlabel:SetLabelHeight(40)
    score.textlabel:SetColor(255,0,0,255)
	score.textlabel:SetShadowColor(255,190,190,190)
	score.textlabel:SetShadowOffset(2,-3)
	score.textlabel:SetShadowBlur(4.0)
    score.score = 0

    function score:IncrementScore()
		score.textlabel:SetColor(255,255,255,255)
		score.flashtime = 0
		score.holdtime = 1
		score:Handle("OnUpdate", FlashNumber)
        self.score = self.score + 1
        self:SetLabel()
        
        if self.score == MAXSCORE then -- finished, someone won
            myScore:Reset()
            opponentScore:Reset()
			if self == myScore then
				ShowPopup("You Win!!!")
			else
				ShowPopup("You lose...") -- This line has the potential to inflict severe mental anguish and suffering
			end
		else
			if self == myScore then
				ShowPopup("You Score!!")
			else
				ShowPopup("Missed it.")
			end	
        end

        ball:Reset()
    end
    
    function score:Reset()
        self.score = 0
        self:SetLabel()
    end
    
    function score:SetLabel()
        self.textlabel:SetLabel(self.score)
    end

    score:Show()
    return score
end

myScore = MakeScoreLabel(160, 80)
opponentScore = MakeScoreLabel(160, 400)

function MakePaddle(x, y, OnUpdate)
    local paddle = Region('Region', 'paddle', UIParent)
    paddle:SetLayer("MEDIUM")
    paddle:SetWidth(120)
    paddle:SetHeight(20)
    paddle:SetAnchor('BOTTOMLEFT', x, y)
    
    function paddle:MoveTo(xCoord, yCoord)
        self:SetAnchor('BOTTOMLEFT', xCoord, yCoord)
    end

    paddle:Handle("OnUpdate",OnUpdate)
    paddle.texture = paddle:Texture("paddle.png")
    paddle.texture:SetBlendMode("BLEND")
    paddle:Show()
    return paddle
end

function Clamp(x)
    if x < 0 then
        x = 0
    elseif x >= 200 then
        x = 200
    end
    return x
end

function UserInputUpdatePosition(self)
    local x,_ = InputPosition()
    self:MoveTo(Clamp(x - self:Width()/2), 10)
end

local tiltspeed = 40.0

function UserTilt(self,x,y,z)
	self:MoveTo(Clamp(self:Left()+tiltspeed*x),10)
end

function AIUpdatePosition(self)
    local centerX,_ = self:Center()
    if ball.x > centerX then
        self.direction = 1
    else
        self.direction = -1
    end
        
    self:MoveTo(Clamp(self:Left() + self.direction * self.velocity), 450)
end
    
myPaddle = MakePaddle(0, 10, UserInputUpdatePosition)
opponentPaddle = MakePaddle(0, 450, AIUpdatePosition)
opponentPaddle.velocity = 3

local accelerate

function ToggleControl(self)
	accelerate = not accelerate
	if accelerate then
		myPaddle:Handle("OnAccelerate", UserTilt)
		myPaddle:Handle("OnUpdate",nil)
	else
		myPaddle:Handle("OnAccelerate", nil)
		myPaddle:Handle("OnUpdate",UserInputUpdatePosition)
	end
end

pongBGRegion:Handle("OnDoubleTap", ToggleControl)
pongBGRegion:EnableInput(true)

---- Ball initialization

ball = Region('Region', 'ball', UIParent)
ball:SetLayer("HIGH")
ball:SetWidth(20)
ball:SetHeight(20)

function ball:MoveTo(xCoord, yCoord)
    self:SetAnchor('BOTTOMLEFT', xCoord, yCoord)
end

function ball:Reset()
    self.x = 0
    self.y = 460
    self.directionX = 1 -- to the right
    self.directionY = -1 -- down
    self.velocity = DEFAULT_VELOCITY
    self:MoveTo(self.x, self.y)
end

function ball:UpdateBallPosition()
    -- side detection
    if self.x <= 0 then
        self.directionX = 1
		upPush:Push(0.0); -- Play
    elseif self.x >= 300 then
        self.directionX = -1
		upPush:Push(0.0); -- Play
    end
    
    -- paddle detection
    if myPaddle:Top() >= self.y and 
        self.x >= (myPaddle:Left() - self:Width()) and 
        self.x <= myPaddle:Right() then
            self.directionY = 1 -- go up
            self.velocity = self.velocity + 0.5
			upPush:Push(0.0); -- Play
    end    
    if opponentPaddle:Bottom() <= self.y and 
        self.x >= (opponentPaddle:Left() - self:Width()) and 
        self.x <= opponentPaddle:Right() then
            self.directionY = -1 -- go down
            self.velocity = self.velocity + 0.5
			upPush:Push(0.0); -- Play
    end
    
    
    -- score detection
    if self.y < 0 then
        opponentScore:IncrementScore()
    elseif self.y > 460 then
        myScore:IncrementScore()
    end

    self.x = self.x + self.directionX * self.velocity
    self.y = self.y + self.directionY * self.velocity
    self:MoveTo(self.x, self.y)

end

ball.texture = ball:Texture("small-ball.png")
ball.texture:SetBlendMode("BLEND")
ball:Reset()
ball:Handle("OnUpdate",ball.UpdateBallPosition)
ball:Show()

pagebutton = Region('Region', 'pagebutton', UIParent)
pagebutton:SetWidth(24)
pagebutton:SetHeight(24)
pagebutton:SetLayer("TOOLTIP")
pagebutton:SetAnchor('BOTTOMLEFT', ScreenWidth() - 28, ScreenHeight() - 28)
pagebutton:EnableClamping(true)
pagebutton:Handle("OnTouchDown", FlipPage)
pagebutton.texture = pagebutton:Texture("circlebutton-16.png")
pagebutton.texture:SetGradientColor("TOP",255,255,255,255,255,255,255,255)
pagebutton.texture:SetGradientColor("BOTTOM",255,255,255,255,255,255,255,255)
pagebutton.texture:SetBlendMode("BLEND")
pagebutton.texture:SetTexCoord(0,1.0,0,1.0)
pagebutton:EnableInput(true)
pagebutton:Show()

upSample = FlowBox("object","mysample", FBSample)
upSample:AddFile("Blue-Mono.wav")
dac = _G["FBDac"]
upSample2 = FlowBox("object","mysample2", FBSample)
upSample2:AddFile("Plick.wav")

upPush = FlowBox("object", "mypush", FBPush)

dac:SetPullLink(0, upSample, 0)

upPush:SetPushLink(0,upSample2, 4)  
upPush:Push(-1.0); -- Turn looping off
upPush:RemovePushLink(0,upSample2, 4)  
upPush:SetPushLink(0,upSample2, 2)
upPush:Push(1.0); -- Set position to end so nothing plays.

dac:SetPullLink(0, upSample2, 0)
StartAudio()
