#pragma once
#include <detail/type_vec3.hpp>
#include <vector>
#include "BoundingBox.h"
#include "Ray.h"
#include "OGLTriangle.h"

class Triangle
{
public:
	glm::vec3 GetCentroid();
	void	  CalculateCentroid();

	glm::vec3 A;
	glm::vec3 B;
	glm::vec3 C;

	glm::vec3 centroid;
};


class KDTreeNode
{
public:

	std::vector<Triangle> nodeTriangles;
	KDTreeNode* leftChild;
	KDTreeNode* rightChild;
	BoundingBox aabb;
};

class KDTreeController
{
public:
	KDTreeController(std::vector<glm::vec3>& sceneMesh);

	KDTreeNode* BuildSceneTree();
	glm::vec3 Hit(glm::vec3 rayOrigin, glm::vec3 rayDirection);

	size_t GetNumVertices(void) const { return mSceneMesh.size(); }
	size_t GetNumTriangles(void) const { return mSceneTriangles.size(); }

	Ray* GetRayOne(void) { return mRayOne; }

	std::vector<std::pair<Triangle, float>>& GetHitTriangles(void) { return mHitTriangles; }

	void DrawBoundingBoxes(KDTreeNode* node, unsigned int depth, AssetManager& assetManager);
	void DrawRays(Camera& m_cam);
	void DrawHitTriangles(Camera& m_cam, AssetManager& assetManager);

	void PrintRootBoundingBox();
	void PrintLeafBoundingBoxes(KDTreeNode* node, AssetManager& assetManager);
	void RenderAABB(AssetManager& assetManager);

	KDTreeNode* root;


private:
	void GetBoundingBox(std::vector<Triangle>& triangles, glm::vec3& min, glm::vec3& max);

	KDTreeNode* BuildTree(std::vector<Triangle> triangleList, int depth, bool maxBBVal, int boundAxis, float val);
	void CalcHitTriangles(KDTreeNode* node, Ray& ray, std::vector<std::pair<Triangle, float>>& hitTriangles);

	std::vector<glm::vec3>& mSceneMesh;
	std::vector<Triangle> mSceneTriangles;
	std::vector<std::pair<Triangle, float>> mHitTriangles;
	std::pair<Triangle, float> mHitTriangleOne;
	std::vector<OGLTriangle> mHitTrianglesView;

	Ray* mRayOne;
};
