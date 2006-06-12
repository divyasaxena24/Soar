#include "BuildFSM.h"
#include "Rectangle.h"

#define MIN_BUILD_UPDATES 10

#define msg cout << "BUILDFSM: "

void BuildFSM::setBuildingInfo(BuildingType type, int centerX, int centerY) {
  int width, height;
  switch (type) {
    case CONTROL_CENTER:
      width = 4 * 16 - 2;
      height = 4 * 16 - 2;
      cost = 600;
      break;
    case BARRACKS:
      width = 4 * 16 - 2;
      height = 3 * 16 - 2;
      cost = 400;
      break;
    case FACTORY:
      width = 4 * 16 - 2;
      height = 3 * 16 - 2;
      cost = 400;
      break;
    default:
      assert(false);
      break; 
  }
  Rectangle r(centerX, centerY, width, height, true);
  buildingBounds = r;
}

BuildFSM::BuildFSM(GameObj* _gob) 
: FSM(_gob)
{ 
  name = OA_BUILD;  
  state = IDLE;
  justStarted = false;
  moveFSM = NULL;
  sgo = NULL;
}

BuildFSM::~BuildFSM() {
  if (moveFSM != NULL) {
    delete moveFSM;
  }
}

void BuildFSM::init(vector<sint4> params) {
  FSM::init(params);
  assert(sgo != NULL);
  type = (BuildingType) params[0];
  loc_x = params[1];
  loc_y = params[2];
  setBuildingInfo(type, loc_x, loc_y);
  state = IDLE;
  buildFrame = -1;
}

int BuildFSM::update() {
  
  int moveStatus;
  Vector<sint4> params;
  Circle unitBounds(*gob->sod.x, *gob->sod.y, *gob->sod.radius);

  switch (state) {
    if (gob->is_pending_action()) {
      msg << "action has not taken effect!\n";
      return FSM_RUNNING;
    }
    case IDLE:
      // begin moving toward build site
      if (moveFSM == NULL) {
        moveFSM = new MoveFSM(gob);
      }
      params.push_back(loc_x);
      params.push_back(loc_y);
      msg << "moving to location.\n";
      moveFSM->init(params);
      moveFSM->update();
      nextState = MOVING;
      break;
    case MOVING:
      moveStatus = moveFSM->update();
      if (moveStatus == FSM_FAILURE) {
        // can't get to the place
        msg << "move has failed!\n";
        return FSM_FAILURE;
      }
      else if (moveStatus == FSM_SUCCESS) {
        msg << "at the site, move done\n";
        nextState = START_BUILD;
      }
      else if (buildingBounds.intersects(unitBounds)) {
        msg << "at the site, stopping moving.\n";
        moveFSM->stop();
        nextState = START_BUILD;
      }
      else if (moveStatus != FSM_RUNNING) {
        // unreachable, probably
        msg << "fail, move returned " << moveStatus << endl;
        return FSM_FAILURE;
      }
      else {
        msg << "movefsm running\n";
      }
      break;
    case START_BUILD:
      if (Sorts::OrtsIO->getBuildAction()) {
        msg << "skipping build: another fsm just started.\n";
      }
      else if (Sorts::OrtsIO->getCurrentMinerals() < cost) {
        msg << "can't afford this building now.\n";
      }
      else {
        Sorts::OrtsIO->setBuildAction();
        justStarted = true;
        
        msg << "starting build.\n";
        buildCycles = 0;
        buildFrame = Sorts::OrtsIO->getViewFrame();
        params.push_back(loc_x);
        params.push_back(loc_y);
        switch (type) {
          case CONTROL_CENTER:
            gob->set_action("build_controlCenter", params);
            break;
          case BARRACKS:
            gob->set_action("build_barracks", params);
            break;
          case FACTORY:
            gob->set_action("build_factory", params);
            break;
        }
        nextState = BUILDING;
      }
      break;
    case BUILDING:
      buildCycles++;
      if (Sorts::OrtsIO->getActionFrame() - buildFrame < 1) {
        msg << "behind a bit.\n";
        return FSM_RUNNING;
      }
      if (justStarted) {
        if (Sorts::OrtsIO->getLastError() != 0) {
          msg << "orts reported an error, failing.\n";
          return FSM_FAILURE;
        }
        justStarted = false;
        sgo->getPerceptualGroup()->setCommandString("build-started");
      }
      if (gob->get_int("is_mobile") == 1) {
        if (buildCycles < MIN_BUILD_UPDATES) {
          msg << "finished impossibly fast.\n";
          return FSM_FAILURE;
        }
        // is this a reliable test for completion?
        msg << "building completed.\n";
        return FSM_SUCCESS;
      }
      break;
  }
  
  state = nextState;
  
  return FSM_RUNNING;
}
