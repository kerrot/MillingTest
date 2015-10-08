/** Example 012 Terrain Rendering

This tutorial will briefly show how to use the terrain renderer of Irrlicht. It
will also show the terrain renderer triangle selector to be able to do
collision detection with terrain.

Note that the Terrain Renderer in Irrlicht is based on Spintz'
GeoMipMapSceneNode, lots of thanks go to him. DeusXL provided a new elegant
simple solution for building larger area on small heightmaps -> terrain
smoothing.

In the beginning there is nothing special. We include the needed header files
and create an event listener to listen if the user presses a key: The 'W' key
switches to wireframe mode, the 'P' key to pointcloud mode, and the 'D' key
toggles between solid and detail mapped material.
*/
#include "irrlicht.h"
#include "driverChoice.h"
#include <list>

using namespace irr;
using namespace video;
using namespace core;
using namespace scene;
using namespace io;
using namespace std;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#define STEP 5
#define TOOL_R 10
#define MATERIAL_SIZE 200
#define OFFSET 13
#define REAL_SIZE (MATERIAL_SIZE + OFFSET + OFFSET)
#define COMP_T 1

IReadFile* file = 0;
ISceneNode* tool = 0;
ICameraSceneNode* camera = 0;
ITerrainSceneNode* node = 0;

float material[REAL_SIZE * REAL_SIZE] = {0};

void calculateNormals(IDynamicMeshBuffer* mb, std::list<std::pair<int, int>>& a_change);
void calculateNormals(IDynamicMeshBuffer* mb, int xstart, int xend, int zstart, int zend);

void Cut()
{
	std::list<std::pair<int, int>> change;

	vector3df toolPos = tool->getPosition();
	
// 	IMesh* m = node->getMesh();
// 	IDynamicMeshBuffer* b = static_cast<IDynamicMeshBuffer*>(m->getMeshBuffer(0));

	IDynamicMeshBuffer* b = static_cast<IDynamicMeshBuffer*>(node->getRenderBuffer());

	

	for (int i = toolPos.X - TOOL_R - 1; i < toolPos.X + TOOL_R + 1; ++i)
	{
		if (i < 0 || i >= MATERIAL_SIZE)
		{
			continue;
		}

		for (int j = toolPos.Z - TOOL_R - 1; j < toolPos.Z + TOOL_R + 1; ++j)
		{
			if (j < 0 || j >= MATERIAL_SIZE)
			{
				continue;
			}

			//change.push_back(std::pair<int, int>(i + OFFSET, j + OFFSET));

			int index = ((i + OFFSET) * REAL_SIZE + j + OFFSET);

			vector3df &pos = b->getVertexBuffer()[index].Pos;
			
			if (pos.Y < toolPos.Y - TOOL_R)
			{
				continue;
			}
			
			float xzD = (i - toolPos.X) * (i - toolPos.X) + (j - toolPos.Z) * (j - toolPos.Z);
			float realY = toolPos.Y - sqrt(TOOL_R * TOOL_R - xzD);

			if (xzD <= TOOL_R * TOOL_R && pos.Y > realY)
			{
				pos.Y = realY;
			}
			
// 			
// 
// 			if (material[index] < toolPos.Y - TOOL_R)
// 			{
// 				continue;
// 			}
// 
// 			float xzD = (i - toolPos.X) * (i - toolPos.X) + (j - toolPos.Z) * (j - toolPos.Z);
// 			float realY = toolPos.Y - sqrt(TOOL_R * TOOL_R - xzD);
// 
// 			if (xzD <= TOOL_R * TOOL_R && material[index] > realY)
// 			{
// 				material[index] = realY;
// 			}
		}
	}

	//calculateNormals(b, change);
	calculateNormals(b, toolPos.X - TOOL_R - COMP_T + OFFSET, toolPos.X + TOOL_R + COMP_T + OFFSET, toolPos.Z - TOOL_R - COMP_T + OFFSET, toolPos.Z + TOOL_R + COMP_T + OFFSET);
	b->setDirty(EBT_VERTEX);
// 	node->setPosition(node->getPosition());
// 	node->setScale(node->getScale());

// 	file->read(material, sizeof(float) * REAL_SIZE * REAL_SIZE);
// 	file->seek(0);
// 	node->loadHeightMapRAW(file, 32, false, true, REAL_SIZE);
// 
// 	node->getMaterial(0).AmbientColor.set(255,0,0,255);
// 	node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
// 	node->setPosition(vector3df(-OFFSET, 0, -OFFSET));
}


void calculateNormals(IDynamicMeshBuffer* mb, std::list<std::pair<int, int>>& a_change)
{
	s32 count;
	core::vector3df a, b, c, t;

	for (std::list<std::pair<int, int>>::iterator iter = a_change.begin();
		iter != a_change.end();
		++iter)
	{
		s32 x = iter->first;
		s32 z = iter->second;

		count = 0;
		core::vector3df normal;

		// top left
		if (x>0 && z>0)
		{
			a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z-1].Pos;
			b = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z].Pos;
			c = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z-1].Pos;
			b = mb->getVertexBuffer()[x*REAL_SIZE+z-1].Pos;
			c = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			count += 2;
		}

		// top right
		if (x>0 && z<REAL_SIZE-1)
		{
			a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z].Pos;
			b = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z+1].Pos;
			c = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z].Pos;
			b = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
			c = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			count += 2;
		}

		// bottom right
		if (x<REAL_SIZE-1 && z<REAL_SIZE-1)
		{
			a = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
			b = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
			c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z+1].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			a = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
			b = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z+1].Pos;
			c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			count += 2;
		}

		// bottom left
		if (x<REAL_SIZE-1 && z>0)
		{
			a = mb->getVertexBuffer()[x*REAL_SIZE+z-1].Pos;
			b = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
			c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			a = mb->getVertexBuffer()[x*REAL_SIZE+z-1].Pos;
			b = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z].Pos;
			c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z-1].Pos;
			b -= a;
			c -= a;
			t = b.crossProduct(c);
			t.normalize();
			normal += t;

			count += 2;
		}

		if (count != 0)
		{
			normal.normalize();
		}
		else
		{
			normal.set(0.0f, 1.0f, 0.0f);
		}

		mb->getVertexBuffer()[x * REAL_SIZE + z].Normal = normal;
	}
}

void calculateNormals(IDynamicMeshBuffer* mb, int xstart, int xend, int zstart, int zend)
{
	s32 count;
	core::vector3df a, b, c, t;

	for (int x = xstart; x < xend; ++x)
	{
		if (x < 0 || x >= REAL_SIZE)
		{
			continue;
		}

		for (int z = zstart; z < zend; ++z)
		{
			if (z < 0 || z >= REAL_SIZE)
			{
				continue;
			}

			count = 0;
			core::vector3df normal;

			// top left
			if (x>0 && z>0)
			{
				a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z-1].Pos;
				b = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z].Pos;
				c = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z-1].Pos;
				b = mb->getVertexBuffer()[x*REAL_SIZE+z-1].Pos;
				c = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				count += 2;
			}

			// top right
			if (x>0 && z<REAL_SIZE-1)
			{
				a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z].Pos;
				b = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z+1].Pos;
				c = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				a = mb->getVertexBuffer()[(x-1)*REAL_SIZE+z].Pos;
				b = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
				c = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				count += 2;
			}

			// bottom right
			if (x<REAL_SIZE-1 && z<REAL_SIZE-1)
			{
				a = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
				b = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
				c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z+1].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				a = mb->getVertexBuffer()[x*REAL_SIZE+z+1].Pos;
				b = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z+1].Pos;
				c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				count += 2;
			}

			// bottom left
			if (x<REAL_SIZE-1 && z>0)
			{
				a = mb->getVertexBuffer()[x*REAL_SIZE+z-1].Pos;
				b = mb->getVertexBuffer()[x*REAL_SIZE+z].Pos;
				c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				a = mb->getVertexBuffer()[x*REAL_SIZE+z-1].Pos;
				b = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z].Pos;
				c = mb->getVertexBuffer()[(x+1)*REAL_SIZE+z-1].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct(c);
				t.normalize();
				normal += t;

				count += 2;
			}

			if (count != 0)
			{
				normal.normalize();
			}
			else
			{
				normal.set(0.0f, 1.0f, 0.0f);
			}

			mb->getVertexBuffer()[x * REAL_SIZE + z].Normal = normal;
		}
	}
}

class MyEventReceiver : public IEventReceiver
{
public:

	MyEventReceiver()
	{
	}

	bool OnEvent(const SEvent& event)
	{
		// check if user presses the key 'W' or 'D'
		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case KEY_KEY_F:
				{
					vector3df p = camera->getPosition();
					printf("P %f, %f, %f\n", p.X, p.Y, p.Z);
					vector3df t = camera->getTarget();
					printf("T %f, %f, %f", t.X, t.Y, t.Z);
				}
				break;

			case KEY_KEY_W:
				{
					tool->setPosition(tool->getPosition() + vector3df(0, 0, STEP));
					Cut();
				}
				break;
			case KEY_KEY_S:
				{
					tool->setPosition(tool->getPosition() + vector3df(0, 0, -STEP));
					Cut();
				}
				break;
			case KEY_KEY_A:
				{
					tool->setPosition(tool->getPosition() + vector3df(-STEP, 0, 0));
					Cut();
				}
				break;
			case KEY_KEY_D:
				{
					tool->setPosition(tool->getPosition() + vector3df(STEP, 0, 0));
					Cut();
				}
				break;
			case KEY_KEY_X:
				{
					tool->setPosition(tool->getPosition() + vector3df(0, -STEP, 0));
					Cut();
				}
				break;
			case KEY_SPACE:
				{
					tool->setPosition(tool->getPosition() + vector3df(0, STEP, 0));
					Cut();
				}
				break;

			default:
				break;
			}
		}

		return false;
	}

private:

};


/*
The start of the main function starts like in most other example. We ask the
user for the desired renderer and start it up. This time with the advanced
parameter handling.
*/
int main()
{
	// create device with full flexibility over creation parameters
	// you can add more parameters if desired, check irr::SIrrlichtCreationParameters
	irr::SIrrlichtCreationParameters params;
	params.DriverType= video::EDT_DIRECT3D9;
	params.WindowSize=core::dimension2d<u32>(1024, 768);
	IrrlichtDevice* device = createDeviceEx(params);

	if (device == 0)
		return 1; // could not create selected driver.

	
	/*
	First, we add standard stuff to the scene: A nice irrlicht engine
	logo, a small help text, a user controlled camera, and we disable
	the mouse cursor.
	*/

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	IFileSystem* filesys = device->getFileSystem();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	SKeyMap keyMap[4];
	keyMap[0].Action = EKA_MOVE_FORWARD;
	keyMap[0].KeyCode = KEY_UP;
	keyMap[1].Action = EKA_MOVE_BACKWARD;
	keyMap[1].KeyCode = KEY_DOWN;
	keyMap[2].Action = EKA_STRAFE_LEFT;
	keyMap[2].KeyCode = KEY_LEFT;
	keyMap[3].Action = EKA_STRAFE_RIGHT;
	keyMap[3].KeyCode = KEY_RIGHT;


	// add camera
	camera = smgr->addCameraSceneNodeFPS(0,100.0f,0.5f, -1, keyMap, 4);

	camera->setPosition(core::vector3df(-168, 283, -168));
	camera->setTarget(core::vector3df(60,137,83));
	camera->setFarValue(42000.0f);

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);

	// create event receiver
	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);

	smgr->setAmbientLight(video::SColorf(0.1,0.1,0.1,1));

	ITerrainSceneNode* floorNode = smgr->addTerrainSceneNode(0, 0, -1, vector3df(0, 0, 0), vector3df(0, 0, 0), vector3df(1, 1, 1), SColor(255, 255, 255, 255), 5, ETPS_17, 0, true);
	char floor[32 * 32] = {0};
	IReadFile* floorfile = filesys->createMemoryReadFile(floor, sizeof(char) * 32 * 32, "m2");
	floorNode->loadHeightMapRAW(floorfile);
	//floorNode->setMaterialFlag(EMF_LIGHTING, false);
	floorNode->setScale(vector3df(100, 1, 100));
	floorNode->setPosition(vector3df(-800, 1, -800));
	floorNode->getMaterial(0).EmissiveColor.set(255,10,10,10);
	//floorNode->setVisible(false);

	smgr->addSphereSceneNode(10);

	tool = smgr->addSphereSceneNode(TOOL_R);
	ISceneNodeAnimator* anim = smgr->createRotationAnimator(core::vector3df(0,1,0));
	tool->addAnimator(anim);
	tool->setPosition(vector3df(0, 120, 0));
	tool->setMaterialFlag(EMF_WIREFRAME, true);

	node = smgr->addTerrainSceneNode(0, 0, -1, vector3df(0, 0, 0), vector3df(0, 0, 0), vector3df(1, 1, 1), SColor(255, 255, 255, 255), 0, ETPS_17, 0, true);
	
	
	
	
	for (int i = OFFSET; i < (MATERIAL_SIZE + OFFSET); ++i)
	{
		for (int j = OFFSET; j < (MATERIAL_SIZE + OFFSET); ++j)
		{
			material[i * REAL_SIZE + j] = 100; 
		}
	}

	file = filesys->createMemoryReadFile(material, sizeof(float) * REAL_SIZE * REAL_SIZE, "m1");
	node->loadHeightMapRAW(file, 32, false, true, REAL_SIZE);

	node->getMaterial(0).AmbientColor.set(255,0,0,255);
	//node->getMaterial(0).Shininess = 20.0f;
	
  	//node->setMaterialFlag(EMF_LIGHTING, false);
 	//node->setMaterialFlag(EMF_WIREFRAME, true);
	node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
	node->setPosition(vector3df(-OFFSET, 0, -OFFSET));


	smgr->addLightSceneNode( 0, core::vector3df(100,300,100), video::SColorf(0.3f,0.3f,0.3f), 300); 

	std::list<vector3df> pathList;
	pathList.push_back(vector3df(30, 100, 30));
	pathList.push_back(vector3df(30, 100, 170));
	pathList.push_back(vector3df(170, 100, 170));
	pathList.push_back(vector3df(170, 100, 30));
	pathList.push_back(vector3df(60, 100, 30));
	pathList.push_back(vector3df(60, 100, 140));
	pathList.push_back(vector3df(140, 100, 140));
	pathList.push_back(vector3df(140, 100, 60));

	std::list<vector3df>::iterator pathIter = pathList.begin();

	int s = 0;


	int lastFPS = -1;

	while(device->run())
	if (device->isWindowActive())
	{
			if (pathIter != pathList.end())
			{
				vector3df endP = *pathIter;
				vector3df noewP = tool->getPosition();

				vector3df vStep;

				if (noewP.X < endP.X)
				{
					vStep.X = STEP;
				}
				if (noewP.X > endP.X)
				{
					vStep.X = -STEP;
				}

				if (noewP.Y < endP.Y)
				{
					vStep.Y = STEP;
				}
				if (noewP.Y > endP.Y)
				{
					vStep.Y = -STEP;
				}

				if (noewP.Z < endP.Z)
				{
					vStep.Z = STEP;
				}
				if (noewP.Z > endP.Z)
				{
					vStep.Z = -STEP;
				}

				tool->setPosition(tool->getPosition() + vStep);
				Cut();

				if (endP.getDistanceFrom(noewP) <= STEP)
				{
					++pathIter;
				}
			}


//  		IMesh* m = node->getMesh();
//  		IMeshBuffer* b = m->getMeshBuffer(0);
//  
//  		u32 c1 = m->getMeshBufferCount();
//  
//  		u32 c2 = b->getVertexCount();
//  
//  		void* v = b->getVertices();
//  
//   		for (int i = 0; i < 30; ++i)
//   		{
//   			for (int j = 0; j < 30; ++j)
//   			{
//  				S3DVertex2TCoords* tmpv = (static_cast<S3DVertex2TCoords*>(v) + (i + s + 10) * 128 + (j + s + 10));
//  
//   				tmpv->Pos.Y = 1000;
//   			}
//   		}
//   
//   		s = (s + 1) % 50 + 10;
//   
//   		for (int i = 0; i < 30; ++i)
//   		{
//   			for (int j = 0; j < 30; ++j)
//   			{
//  				S3DVertex2TCoords* tmpv = (static_cast<S3DVertex2TCoords*>(v) + (i + s + 10) * 128 + (j + s + 10));
//  
//  				tmpv->Pos.Y = 800;
//   			}
//   		}
//  		node->setPosition(node->getPosition());


		driver->beginScene(true, true, 0 );

		smgr->drawAll();
		env->drawAll();


// 		for (int i = 0; i < 30; ++i)
// 		{
// 			for (int j = 0; j < 30; ++j)
// 			{
// 				material[(i + s + 10) * 128 + (j + s + 10)] = 1000;
// 			}
// 		}
// 
// 		s = (s + 1) % 50 + 10;
// 
// 		for (int i = 0; i < 30; ++i)
// 		{
// 			for (int j = 0; j < 30; ++j)
// 			{
// 				material[(i + s + 10) * 128 + (j + s + 10)] = 800;
// 			}
// 		}
// 
// 		file->read(material, sizeof(float) * 128 * 128);
// 		file->seek(0);
// 		node->loadHeightMapRAW(file, 32, false, true, 128);
// 
// 		node->setMaterialFlag(EMF_LIGHTING, false);
// 		node->setMaterialFlag(EMF_WIREFRAME, true);
// 		node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
// 		node->setMaterialFlag(EMF_FRONT_FACE_CULLING, false);

		


		driver->endScene();

		// display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"Terrain Renderer - Irrlicht Engine [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;
			// Also print terrain height of current camera position
			// We can use camera position because terrain is located at coordinate origin

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}

	device->drop();

	return 0;
}

/*
Now you know how to use terrain in Irrlicht.
**/
