#ifndef PerceptualGroupManager_h
#define PerceptualGroupManager_h

#include "SoarGameObject.h"
#include "PerceptualGroup.h"
#include "SoarInterface.h"

class Sorts;


struct ltGroupPtr {
  bool operator()(PerceptualGroup* g1, PerceptualGroup* g2) const {
    int d1 = g1->getDistToFocus();
    int d2 = g2->getDistToFocus();
    if (d1 == d2) {
      // give an arbitrary order if distance is the same
      return ((int)g1 < (int)g2);
    }
    return (d1 < d2);
  }
};

class PerceptualGroupManager {
  public:
    PerceptualGroupManager();
    ~PerceptualGroupManager();

    void initialize();

    void updateGroups();
    bool assignActions();
    void processVisionCommands();

    void makeNewGroup(SoarGameObject* object);
    // used by ORTSInterface when it sees a new object- create a group for it
    
  private:
    VisionParameterStruct visionParams;
    /* in general.h:
    struct VisionParameterStruct {
      int centerX;
      int centerY;
      int viewWidth;
      int focusX;
      int focusY;
      bool ownerGrouping;
      int numObjects;
      int groupingRadius;
    };*/
    
    void prepareForReGroup();
    void reGroup();
    void generateGroupData();
    void adjustAttention(bool rebuildFeatureMaps);

    void removeGroup(PerceptualGroup*);
    void remakeGroupSet();

    // perceptual groups are input to Soar, directly and in feature maps
    // Soar can control how they are created, adjusting the grouping radius
    // and allowing grouping by owner or not.

    // this set is maintained in sorted order, items toward the front
    // are closer to the center of focus, and have priority to go on the
    // input link.
    set <PerceptualGroup*, ltGroupPtr> perceptualGroups;
    
    set <pair<string, int> > staleGroupCategories;
    

    void setAllCategoriesStale();

};

struct perceptualGroupingStruct {
  SoarGameObject* object;
  PerceptualGroup* group;
  bool assigned;
  bool oldGroup;
  int x,y;
};

#endif // PerceptualGroupManager_h
