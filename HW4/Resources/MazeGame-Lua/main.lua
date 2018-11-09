--HW4 Lua Example - Main File
--Stephen J. Guy, 2018

--This lua implemenation of HW4 assumes a C++ host which provides several functions.
--These include:
--  ID = addModel(modelName,x,y,z) creates a new model at (x,y,z), returns ID
--  placeModel(ID,x,y,z) moves existing model ID to location (x,y,z)
--  trnaslateModel(ID,x,y,z) moves model ID by the vector (x,y,z)
--Also, the C++ code automatically calls updateAnimation() and updatePos() each timestep.

-- This only supports very basic features. You should consider adding:
-- a menu system, a win screen, multiple levels, displaying the inventory of keys collected, etc.

print("Maze Walkthough Game (Lua)")
print("Up/Down keys - moves forward/backwards")
print("Left/Right keys - turns left/right")
print("Shift & Left / Shift & Right - strafes")
print("Z - restarts level")

require "MazeGame/Camera"          -- Camera Variables
require "MazeGame/MapLoader"       -- Parse map files
require "MazeGame/AnimationSystem" -- A simple animation system

dt = 0 --Time per frame, updated by updateAnimation()
m_PI = 3.141592

--Character Atributes
angleSpeed = 1.8
linSpeed = 1.8
playerRadius = .2
pickupRadius = .22

--Initalize player inventory to be emtpy
activeKeyType = nil; --Which key is active
hasItem = {}         --Player inventory


--updateAnimation() is called once per frame. cur_dt is  how much time has 
--  passed since last frame. Here we update the position and orientation of 
--  any animated model based on it's velocity and rotational velocity.
--  Any model who is done animated has it's onDone() function called.
function updateAnimation(cur_dt)
  dt = cur_dt --Update global 'dt' 
  updateAnimationSystem(dt) --Update our internal anymation system (rotate & translate models)
end

--Check if a given position would be okay for the player to move to (ie, not colliding
--  with any walls or doors). We don't just check if the player's center is colliding,
--  but also check a radius around the agent.
--  TODO: We end up treating the agent as a square more than a circle...
--        ...this casues us problems with corners. =/
function isWalkable(newX,newY)
  for dx = -1, 1, 2 do --Test left right extents of player for collisions
    for dy = -1, 1, 2 do
      i = math.floor(newX+playerRadius*dx)
      j = math.floor(newY+playerRadius*dy)
      if i < 1 or j < 1 or i > mapSize[1] or j > mapSize[2] then
        return false
      end
      type = map[i][j]
      if isDoor[type] or isWall[type] then
        return false
      end
    end
  end
  return true
end

--Compute 2D dot product
function dotProd(a, b)
  return a[1]*b[1] + a[2]*b[2]
end

--Return an normalized version of a vector
function normalize(a)
  local len = math.sqrt(a[1]*a[1] + a[2]*a[2])
  return {a[1]/len, a[2]/len}
end

--This is called each time the player moves. Here, we need to see if they pick up any 
--keys or unlock any doors.
function checkForEvents()
  --We know the player can only interact with objects in one of the 9 neighboring cells
  --  so we only check the 8 extreme points of the agent (plus the center).
  --  Note: this assumes pickupRadius < cell size
  for dx = -1, 1, 2 do 
    for dy = -1, 1, 2 do

      --Compute the i,j coordinates for the extream point (dx,dy)
      local i = math.floor(CameraPosX+pickupRadius*dx)
      local j = math.floor(CameraPosY+pickupRadius*dy)
      if i < 1 or j < 1 or i > mapSize[1] or j > mapSize[2] then --ignore cells off edge of map
        goto continue --Oops! Lua doesn't have a continue statement...
      end

      --Check if the center of this cell is directly in front of us (angle to cell < pi/8).
      --   If not, skip it. We don't want to pickup keys or unlock doors that we are not
      --   looking directly at.
      local toGrid = normalize({i+.5 - CameraPosX, j+.5 - CameraPosY}) -- Vector to center of grid
      local camDir = normalize({CameraDirX,CameraDirY}) --TODO: Probably we can just normalize this once
      local angToGrid = math.acos(dotProd(toGrid, camDir))
      if math.abs(angToGrid) > m_PI/8 then
        goto continue
      end

      local type = map[i][j] --Get object type

      --If the player is overlapping a key pick the key up.
      --We also hide the model of the current key under the floor, so the player can
      --  no longer see it (it's been 'picked up'!).
      if isKey[type] then
        hasItem[type] = true
        map[i][j] = 0 --delete key from map
        translateModel(modelID[i][j],0,0,-1) --hide the key under the floor
        activeKeyType = type
        setModelToMaterial(caryKey,activeKeyType)
      end

      --If you are looking at a door for which you have the key (and the door
      --is not playing its opening animation), then animate the door opening.
      --We create an opening animation by setting the velocity to -1 in z (sinking
      --the door into the floor). We also set a custom trigger when the door is
      --done animating to delete the door from the map.
      local id = modelID[i][j]
      if isDoor[type] and hasItem[string.lower(type)] and not animatedModels[id] then
        animatedModels[id] = true
        velModel[id] = {0,0,-1}
        timeToMove[id] = 1
        onDone[id] = function () --The function to run when time is up
            map[i][j] = 0;  --delete door from map
          end
      end

      ::continue:: --We use must use a 'goto' to simulate continue in Lua
    end
  end
end

--When the user presses direcional keys change their velocity. Then, each timestep
--update the player's position based on their velocity. This allows much smoother
--animation than updating the camera only on key presses!
function updatePos(keys)
  if keys.z_PressedEvent then --Reset level if 'z' is pressed
    print("Resetting Level!")
    resetLevel()
  end

  --Update the player's velocity and angle based on which keys are pressed
  local vX = 0
  local vY = 0
  local w = 0 --angular velocity
  if keys.upPressed then
    vX = vX + linSpeed*CameraDirX
    vY = vY + linSpeed*CameraDirY
  end
  if keys.downPressed then
    vX = vX - linSpeed*CameraDirX
    vY = vY - linSpeed*CameraDirY
  end
  if keys.rightPressed then
    if keys.shiftPressed then
      vX = vX + linSpeed*CameraDirY
      vY = vY - linSpeed*CameraDirX
    else
      w = w - angleSpeed 
    end
  end
  if keys.leftPressed then
    if keys.shiftPressed then
      vX = vX - linSpeed*CameraDirY
      vY = vY + linSpeed*CameraDirX
    else
      w = w + angleSpeed
    end
  end

  --Update camera orientation
  CameraAngle = CameraAngle + w*dt
  setCamDirFromAngle(CameraAngle)

  --Only update camera position if the new position will be collision free 
  newX = CameraPosX + vX*dt
  newY = CameraPosY + vY*dt
  if isWalkable(newX,newY) then
    CameraPosX = newX
    CameraPosY = newY
  end

  --If the player is holding a key, draw it
  if activeKeyType then
    placeModel(caryKey,CameraPosX+CameraDirX*.3,CameraPosY+CameraDirY*.3,0)
    rotateModel(caryKey,CameraAngle,0,0,1)
    scaleModel(caryKey,.3,.3,.3)
    translateModel(caryKey,0,0,.1)
  end

  checkForEvents() --Check to see if our movement has triggered any events
end

--Set-up level to initial values stored in savedMap. This involves resetting
--the inventory, camera, and animation system as well as setting all the models
--to their starting positions.
function resetLevel()
  --Set the players inventory to emtpy
  activeKeyType = nil; 
  hasItem = {} 
  placeModel(caryKey,0,0,-10) --hide active key model underground, only show this
                              --key when the player is carying it

  --Initalize the animation sysetm
  resetAnimationSystem()

  --Reset map from savedMap and place models at inital positions, and
  --set the camera
  for i = 1, mapSize[2] do
    for j = 1, mapSize[1] do
      local c = savedMap[i][j]
      map[i][j] = savedMap[i][j]
      
      local x = i
      local y = j
      local z = 0
      if isDoor[c] then  --Draw door with right material
        placeModel(modelID[i][j],x+.5,y+.5,z)
      end
      if isKey[c] then   --Draw key with material that matches door
        id = modelID[i][j]
        placeModel(id,x+.5,y+.5,z)
        rotZVelModel[id] = .7 --Set the key spinning
        animatedModels[id] = true --We must add the item to the table of animated models
      end
      if c == "S" then  -- Player Start Location (set camera)
        --Set the camera to the starting position
        CameraPosX = x+.5
        CameraPosY = y+.5
        --Set the camera to the the starting orentation
        CameraAngle = 0
        setCamDirFromAngle(CameraAngle)
      end
    end
  end
end

--Load the map file to start the game
local file = "level1.txt"
loadMap(file)