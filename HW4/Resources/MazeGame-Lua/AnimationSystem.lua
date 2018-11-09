--HW4 Lua Example - Animation System
--Stephen J. Guy, 2018

--This file creates a simple animation system that allows models to translate
--or rotate (around the z-axis) for a fixed amount of time. Once that time
--has passed, the animatin will be stopped, and the corisponding onDone function 
--will be called if it exists.  This allows us to trigger somethign to happen when
--the animation finishes. (E.g., only mark a door as open once the animation has played)

--Animation System Variables
animatedModels = {}  --Which models are animated
velModel = {}        --Linear velocity for each animated model
rotZVelModel = {}    --Angular (z) velocity for each animated model
timeToMove = {}      --How long to play animation translating/roating model ('nil' means forever)
onDone = {}          --Function to call when timeToMove is up

--updateAnimation() should be called once per frame. cur_dt is how much time has 
--  passed since last frame. Here we update the position and orientation of 
--  any animated model based on it's velocity and rotational velocity.
--  Any model who is done animated has it's onDone() function called.
function updateAnimationSystem(cur_dt)  
  --Loop over all animated models, updating their position/rotation
  for modelID,v in pairs(animatedModels) do

    --Update position for models with any velocity
    local vel = velModel[modelID]
    if vel then 
      translateModel(modelID,dt*vel[1],dt*vel[2],dt*vel[3])
    end

    --Update orientation for models with any rotational velocity
    local rotZvel = rotZVelModel[modelID]
    if rotZvel then 
      rotateModel(modelID,rotZvel*dt, 0, 0, 1)
    end

    --Check if animation has time limit, if so decrement time remaining.
    --If the animation is done, call the onDone() function for that animation.
    if timeToMove[modelID] ~= nil then 
      timeToMove[modelID] = timeToMove[modelID] - dt
      if timeToMove[modelID] <= 0 then
        if onDone[modelID] then
          onDone[modelID]() -- Call the on completion event for this animation
        end
        animatedModels[modelID] = nil --delete model from the list of animated models
      end
    end
  end
end

function resetAnimationSystem()
  animatedModels = {} 
  velModel = {}
  rotZVelModel = {}
  timeToMove = {}
  onDone = {} 
end