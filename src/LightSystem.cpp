#include "LTBL/LightSystem.h"

#include "LTBL/ShadowFin.h"

#include <assert.h>

using namespace ltbl;
using namespace qdt;

const sf::Color clearColor(0, 0, 0, 0);

EmissiveLight::EmissiveLight()
	: scale(1.0f, 1.0f)
{
}

void EmissiveLight::SetTexture(sf::Texture* texture)
{
	text = texture;

	// Update aabb
	float halfWidth = text->getSize().x / 2.0f;
	float halfHeight = text->getSize().y / 2.0f;

	aabb.lowerBound.x = center.x - halfWidth;
	aabb.lowerBound.y = center.y - halfHeight;
	aabb.upperBound.x = center.x + halfWidth;
	aabb.upperBound.y = center.y + halfHeight;

	UpdateTreeStatus();
}

void EmissiveLight::Render()
{
	glPushMatrix();

	glTranslatef(center.x, center.y, 0.0f);
	glScalef(scale.x, scale.y, 1.0f);

	DrawQuad(*text);

	glPopMatrix();
}

void EmissiveLight::SetCenter(const Vec2f &newCenter)
{
	Vec2f difference = newCenter - center;

	center = newCenter;

	aabb.lowerBound += difference;
	aabb.upperBound += difference;
}

void EmissiveLight::IncCenter(const Vec2f &increment)
{
	center += increment;
	aabb.lowerBound += increment;
	aabb.upperBound += increment;
}

void EmissiveLight::SetScale(const Vec2f &newScale)
{
	scale = newScale;
	aabb.SetDims(Vec2f(text->getSize().x / 2.0f * scale.x, text->getSize().y / 2.0f * scale.y));
}

Vec2f EmissiveLight::GetScale()
{
	return scale;
}

Vec2f EmissiveLight::GetCenter()
{
	return center;
}

void EmissiveLight::Update()
{
	UpdateTreeStatus();
}

LightSystem::LightSystem(const AABB &region, sf::RenderWindow* pRenderWindow)
	: ambientColor(0, 0, 0), checkForHullIntersect(true),
	prebuildTimer(0), pWin(pRenderWindow)
{
	view.setCenter(sf::Vector2f(0.0f, 0.0f));
	view.setSize(sf::Vector2f(static_cast<float>(pRenderWindow->getSize().x), static_cast<float>(pRenderWindow->getSize().y)));

	// Load the soft shadows texture
	if(!softShadowTexture.loadFromFile("data/softShadowsTexture.png"))
		abort(); // Could not find the texture, abort

	SetUp(region);
}

LightSystem::~LightSystem()
{
	// Destroy resources
	ClearLights();
	ClearConvexHulls();
	ClearEmissiveLights();
}

void LightSystem::CameraSetup()
{
	sf::Vector2f viewCenter = view.getCenter();
	glTranslatef(-viewCenter.x, -viewCenter.y, 0.0f);
}

void LightSystem::MaskShadow(Light* light, ConvexHull* convexHull, float depth)
{
	// ----------------------------- Determine the Shadow Boundaries -----------------------------

	Vec2f lCenter = light->center;
	float lRadius = light->radius;

	Vec2f hCenter = convexHull->GetWorldCenter();

	const int numVertices = convexHull->vertices.size();

	std::vector<bool> backFacing(numVertices);

	for(int i = 0; i < numVertices; i++)
    {
		Vec2f firstVertex(convexHull->GetWorldVertex(i));
        int secondIndex = (i + 1) % numVertices;
        Vec2f secondVertex(convexHull->GetWorldVertex(secondIndex));
        Vec2f middle = (firstVertex + secondVertex) / 2.0f;

		// Use normal to take light width into account, this eliminates popping
		Vec2f lightNormal(-(lCenter.y - middle.y), lCenter.x - middle.x);

		Vec2f centerToBoundry = middle - hCenter;

		if(centerToBoundry.dot(lightNormal) < 0)
			lightNormal *= -1;

		lightNormal = lightNormal.normalize() * light->size;

		Vec2f L = (lCenter - lightNormal) - middle;
                
		if (convexHull->normals[i].dot(L) > 0)
            backFacing[i] = false;
        else
            backFacing[i] = true;
    }

	int firstBoundryIndex = 0;
	int secondBoundryIndex = 0;

	for(int currentEdge = 0; currentEdge < numVertices; currentEdge++)
    {
        int nextEdge = (currentEdge + 1) % numVertices;

        if (backFacing[currentEdge] && !backFacing[nextEdge])
            firstBoundryIndex = nextEdge;

        if (!backFacing[currentEdge] && backFacing[nextEdge])
            secondBoundryIndex = nextEdge;
    }

	// -------------------------------- Shadow Fins --------------------------------

	Vec2f firstBoundryPoint = convexHull->GetWorldVertex(firstBoundryIndex);

	Vec2f lightNormal(-(lCenter.y - firstBoundryPoint.y), lCenter.x - firstBoundryPoint.x);

	Vec2f centerToBoundry = firstBoundryPoint - hCenter;

	if(centerToBoundry.dot(lightNormal) < 0)
		lightNormal *= -1;

	lightNormal = lightNormal.normalize() * light->size;

	ShadowFin firstFin;

	firstFin.rootPos = firstBoundryPoint;
	firstFin.umbra = firstBoundryPoint - (lCenter + lightNormal);
	firstFin.umbra = firstFin.umbra.normalize() * lRadius;

	firstFin.penumbra = firstBoundryPoint - (lCenter - lightNormal);
	firstFin.penumbra = firstFin.penumbra.normalize() * lRadius;

	ShadowFin secondFin;

	Vec2f secondBoundryPoint = convexHull->GetWorldVertex(secondBoundryIndex);

	lightNormal = Vec2f(-(lCenter.y - secondBoundryPoint.y), lCenter.x - secondBoundryPoint.x);
	centerToBoundry = secondBoundryPoint - hCenter;

	if(centerToBoundry.dot(lightNormal) < 0)
		lightNormal *= -1;

	lightNormal = lightNormal.normalize() * light->size;

	secondFin.rootPos = secondBoundryPoint;
	secondFin.umbra = secondBoundryPoint - (lCenter + lightNormal);
	secondFin.umbra = secondFin.umbra.normalize() * lRadius;
	
	secondFin.penumbra = secondBoundryPoint - (lCenter - lightNormal);
	secondFin.penumbra = secondFin.penumbra.normalize() * lRadius;

	// Store generated fins to render later
	finsToRender.push_back(firstFin);
	finsToRender.push_back(secondFin);

	Vec2f mainUmbraRoot1 = firstFin.rootPos;
	Vec2f mainUmbraRoot2 = secondFin.rootPos;
	Vec2f mainUmbraVec1 = firstFin.umbra;
	Vec2f mainUmbraVec2 = secondFin.umbra;

	AddExtraFins(*convexHull, &firstFin, *light, mainUmbraVec1, mainUmbraRoot1, firstBoundryIndex, false);
	AddExtraFins(*convexHull, &secondFin, *light, mainUmbraVec2, mainUmbraRoot2, secondBoundryIndex, true);
	
	// ----------------------------- Drawing the umbra -----------------------------

	Vec2f vertex1 = convexHull->GetWorldVertex(firstBoundryIndex);
	Vec2f vertex2 = convexHull->GetWorldVertex(secondBoundryIndex);

	Vec2f throughCenter = (hCenter - lCenter).normalize() * lRadius;

	// 3 rays is enough in most cases
	glBegin(GL_TRIANGLE_STRIP);

	glVertex3f(mainUmbraRoot1.x, mainUmbraRoot1.y, depth);
	glVertex3f(mainUmbraRoot1.x + mainUmbraVec1.x, mainUmbraRoot1.y + mainUmbraVec1.y, depth);
	glVertex3f(hCenter.x, hCenter.y, depth);
	glVertex3f(hCenter.x + throughCenter.x, hCenter.y + throughCenter.y, depth);
	glVertex3f(mainUmbraRoot2.x, mainUmbraRoot2.y, depth);
	glVertex3f(mainUmbraRoot2.x + mainUmbraVec2.x, mainUmbraRoot2.y + mainUmbraVec2.y, depth);

	glEnd();
}

void LightSystem::AddExtraFins(const ConvexHull &hull, ShadowFin* fin, const Light &light, Vec2f &mainUmbra, Vec2f &mainUmbraRoot, int boundryIndex, bool wrapCW)
{
	Vec2f hCenter = hull.GetWorldCenter();

	int secondEdgeIndex;
	int numVertices = static_cast<signed>(hull.vertices.size());

	for(int i = 0; i < maxFins; i++)
	{	
		if(wrapCW)
			secondEdgeIndex = Wrap(boundryIndex - 1, numVertices);
		else
			secondEdgeIndex = Wrap(boundryIndex + 1, numVertices);

		Vec2f edgeVec = Vec2f(hull.vertices[secondEdgeIndex].position - hull.vertices[boundryIndex].position).normalize();

		Vec2f penNorm(fin->penumbra.normalize());

		float angle1 = acosf(penNorm.dot(edgeVec) / (fin->penumbra.magnitude() * edgeVec.magnitude()));
		float angle2 = acosf(penNorm.dot(fin->umbra) / (fin->penumbra.magnitude() * fin->umbra.magnitude()));

		if(angle1 >= angle2)
			break; // No intersection, break

		// Change existing fin to attatch to side of hull
		fin->umbra = edgeVec * light.radius;

		// Add the extra fin
		Vec2f secondBoundryPoint = hull.GetWorldVertex(secondEdgeIndex);

		Vec2f lightNormal(-(light.center.y - secondBoundryPoint.y), light.center.x - secondBoundryPoint.x);

		Vec2f centerToBoundry = secondBoundryPoint - hCenter;

		if(centerToBoundry.dot(lightNormal) < 0)
			lightNormal *= -1;

		lightNormal = lightNormal.normalize() * light.size;

		ShadowFin newFin;

		newFin.rootPos = secondBoundryPoint;
		newFin.umbra = secondBoundryPoint - (light.center + lightNormal);
		newFin.umbra = newFin.umbra.normalize() * light.radius;
		newFin.penumbra = edgeVec.normalize() * light.radius;

		finsToRender.push_back(newFin);

		fin = &finsToRender.back();

		boundryIndex = secondEdgeIndex;

		break;
	}

	// Change the main umbra to correspond to the last fin
	mainUmbraRoot = fin->rootPos;
	mainUmbra = fin->umbra;
}

void LightSystem::SetUp(const AABB &region)
{
	// Create the quad trees
	lightTree.reset(new QuadTree(region));
	hullTree.reset(new QuadTree(region));
	emissiveTree.reset(new QuadTree(region));

	sf::Vector2f viewSize(view.getSize());
	sf::Vector2u viewSizeui(static_cast<unsigned int>(viewSize.x), static_cast<unsigned int>(viewSize.y));

	renderTexture.create(viewSizeui.x, viewSizeui.y, false);
	renderTexture.setSmooth(true);

	lightTemp.create(viewSizeui.x, viewSizeui.y, true);
	lightTemp.setSmooth(true);

	InitGlew();

	// Check to see if advanced blending is supported
	if(!GL_ARB_imaging)
	{
		std::cout << "Advanced blending not supported on this machine!" << std::endl;
		abort();
	}

	// -------------------------- Prepare the main render texture --------------------------

	renderTexture.setActive();

	glViewport(0, 0, viewSizeui.x, viewSizeui.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewSizeui.x, 0, viewSizeui.y, -100.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// Disable textures, enabled only temporarily for soft shadow texture
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);

	// -------------------------- Prepare the light texture --------------------------

	lightTemp.setActive();

	glViewport(0, 0, viewSizeui.x, viewSizeui.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewSizeui.x, 0, viewSizeui.y, -100.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	 // Use normal depth oder testing

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// Disable textures, enabled only temporarily for soft shadow texture
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);

	pWin->setActive();
}

void LightSystem::AddLight(Light* newLight)
{
	newLight->pWin = pWin;
	lights.insert(newLight);
	lightTree->AddOccupant(newLight);
}

void LightSystem::AddConvexHull(ConvexHull* newConvexHull)
{
	convexHulls.insert(newConvexHull);
	hullTree->AddOccupant(newConvexHull);
}

void LightSystem::AddEmissiveLight(EmissiveLight* newEmissiveLight)
{
	emissiveLights.insert(newEmissiveLight);
	emissiveTree->AddOccupant(newEmissiveLight);
}

void LightSystem::RemoveLight(Light* pLight)
{
	boost::unordered_set<Light*>::iterator it = lights.find(pLight);

	assert(it != lights.end());

	(*it)->RemoveFromTree();

	lights.erase(it);
}

void LightSystem::RemoveConvexHull(ConvexHull* pHull)
{
	boost::unordered_set<ConvexHull*>::iterator it = convexHulls.find(pHull);

	assert(it != convexHulls.end());

	(*it)->RemoveFromTree();

	convexHulls.erase(it);
}

void LightSystem::RemoveEmissiveLight(EmissiveLight* pEmissiveLight)
{
	boost::unordered_set<EmissiveLight*>::iterator it = emissiveLights.find(pEmissiveLight);

	assert(it != emissiveLights.end());

	(*it)->RemoveFromTree();

	emissiveLights.erase(it);
}

void LightSystem::ClearLights()
{
	// Delete contents
	for(boost::unordered_set<Light*>::iterator it = lights.begin(); it != lights.end(); it++)
		delete *it;

	lights.clear();

	if(lightTree.get() != NULL)
		lightTree->ClearTree(AABB(Vec2f(-50.0f, -50.0f), Vec2f(-50.0f, -50.0f)));
}

void LightSystem::ClearConvexHulls()
{
	// Delete contents
	for(boost::unordered_set<ConvexHull*>::iterator it = convexHulls.begin(); it != convexHulls.end(); it++)
		delete *it;

	convexHulls.clear();

	if(hullTree.get() != NULL)
		hullTree->ClearTree(AABB(Vec2f(-50.0f, -50.0f), Vec2f(-50.0f, -50.0f)));
}

void LightSystem::ClearEmissiveLights()
{
	// Delete contents
	for(boost::unordered_set<EmissiveLight*>::iterator it = emissiveLights.begin(); it != emissiveLights.end(); it++)
		delete *it;

	emissiveLights.clear();

	if(emissiveTree.get() != NULL)
		emissiveTree->ClearTree(AABB(Vec2f(-50.0f, -50.0f), Vec2f(-50.0f, -50.0f)));
}

void LightSystem::RenderLights()
{
	lightTemp.setActive();
	glLoadIdentity();
	CameraSetup();

	// Bind shadow texture for use later
  sf::Texture::bind(&softShadowTexture);

	renderTexture.setActive();
	renderTexture.clear(ambientColor);
	glLoadIdentity();
	CameraSetup();

	renderTexture.pushGLStates();

	// Get visible lights
	sf::Vector2f viewCenter = view.getCenter();
	sf::Vector2f viewSize = view.getSize();

	AABB view(Vec2f(viewCenter.x, viewCenter.y),
		Vec2f(viewSize.x + viewCenter.x, viewSize.y + viewCenter.y));

	std::vector<QuadTreeOccupant*> visibleLights;
	lightTree->Query(view, visibleLights);

	// Add lights from pre build list if there are any
	if(!lightsToPreBuild.empty())
	{
		if(prebuildTimer < 2)
		{
			prebuildTimer++;

			const unsigned int numLightsToPreBuild = lightsToPreBuild.size();

			for(unsigned int i = 0; i < numLightsToPreBuild; i++)
			{
				lightsToPreBuild[i]->updateRequired = true;
				visibleLights.push_back(lightsToPreBuild[i]);
			}
		}
		else
			lightsToPreBuild.clear();
	}

	const unsigned int numVisibleLights = visibleLights.size();

	for(unsigned int l = 0; l < numVisibleLights; l++)
	{
		Light* pLight = static_cast<Light*>(visibleLights[l]);

		bool updateRequired = false;

		if(pLight->AlwaysUpdate())
			updateRequired = true;
		else if(pLight->updateRequired)
			updateRequired = true;

		// Get hulls that the light affects
		std::vector<QuadTreeOccupant*> regionHulls;
		hullTree->Query(*pLight->GetAABB(), regionHulls);

		const unsigned int numHulls = regionHulls.size();

		if(!updateRequired)
		{
			// See of any of the hulls need updating
			for(unsigned int h = 0; h < numHulls; h++)
			{
				ConvexHull* pHull = static_cast<ConvexHull*>(regionHulls[h]);

				if(pHull->updateRequired)
				{
					pHull->updateRequired = false;
					updateRequired = true;
					break;
				}
			}
		}

		if(updateRequired)
		{
			Vec2f staticTextureOffset;

			// Activate the intermediate render Texture
			if(pLight->AlwaysUpdate())
				lightTemp.setActive();
			else
			{
				pLight->pStaticTexture->setActive();

				// For use later
        sf::Texture::bind(&softShadowTexture);

				staticTextureOffset = pLight->center - pLight->aabb.lowerBound;

				glLoadIdentity();
				glTranslatef(-pLight->center.x + staticTextureOffset.x, -pLight->center.y + staticTextureOffset.y, 0.0f);
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Disable color and alpha buffer writes temporarily for masking
			glColorMask(false, false, false, false);

			if(checkForHullIntersect)
				for(unsigned int h = 0; h < numHulls; h++)
				{
					ConvexHull* pHull = static_cast<ConvexHull*>(regionHulls[h]);

					Vec2f hullToLight(pLight->center - pHull->GetWorldCenter());
					hullToLight = hullToLight.normalize() * pLight->size;

					if(!pHull->PointInsideHull(pLight->center - hullToLight))
						MaskShadow(pLight, pHull, 2.0f);
				}
			else
				for(unsigned int h = 0; h < numHulls; h++)
					MaskShadow(pLight, static_cast<ConvexHull*>(regionHulls[h]), 2.0f);

			// Render the hulls only for the hulls that had
			// there shadows rendered earlier (not out of bounds)
			for(unsigned int h = 0; h < numHulls; h++)
				static_cast<ConvexHull*>(regionHulls[h])->RenderHull(2.0f);

			glBlendFunc(GL_ONE, GL_ONE);

			// Re-enable color buffer and alpha buffer writes
			glColorMask(true, true, true, true);

			// Render the current light
			pLight->RenderLightSolidPortion(1.0f);

			// Color reset
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			// Render shadow fins, multiply alpha
			glEnable(GL_TEXTURE_2D);

			glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

			const unsigned int numFins = finsToRender.size();

			for(unsigned int f = 0; f < numFins; f++)
				finsToRender[f].Render(1.0f);

			// Soft light angle fins
			pLight->RenderLightSoftPortion(1.0f);

			glDisable(GL_TEXTURE_2D);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			// Now render that intermediate render Texture to the main render Texture
			if(pLight->AlwaysUpdate())
			{
				lightTemp.display();

				renderTexture.setActive();

        sf::Texture::bind(&lightTemp.getTexture());

				// Save the camera matrix
				glLoadIdentity();

				glBlendFunc(GL_ONE, GL_ONE);

				glBegin(GL_QUADS);
					glTexCoord2i(0, 0); glVertex3f(0.0f, 0.0f, 0.0f);
					glTexCoord2i(1, 0); glVertex3f(viewSize.x, 0.0f, 0.0f);
					glTexCoord2i(1, 1); glVertex3f(viewSize.x, viewSize.y, 0.0f);
					glTexCoord2i(0, 1); glVertex3f(0.0f, viewSize.y, 0.0f);
				glEnd();
			}
			else
			{
				pLight->pStaticTexture->display();

				renderTexture.setActive();

        sf::Texture::bind(&pLight->pStaticTexture->getTexture());

				glTranslatef(pLight->center.x - staticTextureOffset.x, pLight->center.y - staticTextureOffset.y, 0.0f);

				glBlendFunc(GL_ONE, GL_ONE);

				const float lightStaticTextureWidth = static_cast<float>(pLight->pStaticTexture->getSize().x);
				const float lightStaticTextureHeight = static_cast<float>(pLight->pStaticTexture->getSize().y);

				glBegin(GL_QUADS);
					glTexCoord2i(0, 0); glVertex3f(0.0f, 0.0f, 0.0f);
					glTexCoord2i(1, 0); glVertex3f(lightStaticTextureWidth, 0.0f, 0.0f);
					glTexCoord2i(1, 1); glVertex3f(lightStaticTextureWidth, lightStaticTextureHeight, 0.0f);
					glTexCoord2i(0, 1); glVertex3f(0.0f, lightStaticTextureHeight, 0.0f);
				glEnd();
			}

			pLight->updateRequired = false;
		}
		else
		{
			// Render existing texture
      sf::Texture::bind(&pLight->pStaticTexture->getTexture());

			Vec2f staticTextureOffset = pLight->center - pLight->aabb.lowerBound;

			glTranslatef(pLight->center.x - staticTextureOffset.x, pLight->center.y - staticTextureOffset.y, 0.0f);

			glBlendFunc(GL_ONE, GL_ONE);

			const float lightStaticTextureWidth = static_cast<float>(pLight->pStaticTexture->getSize().x);
			const float lightStaticTextureHeight = static_cast<float>(pLight->pStaticTexture->getSize().y);

			glBegin(GL_QUADS);
				glTexCoord2i(0, 0); glVertex3f(0.0f, 0.0f, 0.0f);
				glTexCoord2i(1, 0); glVertex3f(lightStaticTextureWidth, 0.0f, 0.0f);
				glTexCoord2i(1, 1); glVertex3f(lightStaticTextureWidth, lightStaticTextureHeight, 0.0f);
				glTexCoord2i(0, 1); glVertex3f(0.0f, lightStaticTextureHeight, 0.0f);
			glEnd();
		}

		regionHulls.clear();
		finsToRender.clear();
	}

	// Emissive lights
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<QuadTreeOccupant*> visibleEmissiveLights;

	emissiveTree->Query(view, visibleEmissiveLights);

	const unsigned int numEmissiveLights = visibleEmissiveLights.size();

	for(unsigned int i = 0; i < numEmissiveLights; i++)
		static_cast<EmissiveLight*>(visibleEmissiveLights[i])->Render();

	// Reset
	renderTexture.popGLStates();

	renderTexture.display();

	pWin->setActive();
}

void LightSystem::BuildLight(Light* pLight)
{
	lightsToPreBuild.push_back(pLight);
}

void LightSystem::RenderLightTexture(float renderDepth)
{
	sf::Vector2f viewSize(view.getSize());
	sf::Vector2u viewSizeui(static_cast<unsigned int>(viewSize.x), static_cast<unsigned int>(viewSize.y));

	pWin->resetGLStates();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);						

	glViewport(0, 0, viewSizeui.x, viewSizeui.y);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D); 
	glDisable(GL_DEPTH_TEST);

  sf::Texture::bind(&renderTexture.getTexture());

	// Set up color function to multiply the existing color with the render texture color
	glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Seperate allows you to set color and alpha functions seperately

	glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex3f(0.0f, 0.0f, renderDepth);
		glTexCoord2i(1, 1); glVertex3f(viewSize.x, 0.0f, renderDepth);
		glTexCoord2i(1, 0); glVertex3f(viewSize.x, viewSize.y, renderDepth);
		glTexCoord2i(0, 0); glVertex3f(0.0f, viewSize.y, renderDepth);
	glEnd();

	pWin->resetGLStates();
}
