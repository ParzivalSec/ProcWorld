#include "KDTreeController.h"
#include <algorithm>
#include "MathUtil.h"

glm::vec3 Triangle::GetCentroid()
{
	return centroid;
}

void Triangle::CalculateCentroid()
{
	centroid.x = (A.x + B.x + C.x) / 3.0f;
	centroid.y = (A.y + B.y + C.y) / 3.0f;
	centroid.z = (A.z + B.z + C.z) / 3.0f;
}

KDTreeController::KDTreeController(std::vector<glm::vec3>& sceneMesh) 
	: mSceneMesh(sceneMesh)
	, mHitTriangleOne(Triangle(), 0.0f)
	, mRayOne(nullptr)
{
	for (GLuint i = 0; i < mSceneMesh.size(); i += 3)
	{
		Triangle tri;

		tri.A = mSceneMesh[i];
		tri.B = mSceneMesh[i + 1];
		tri.C = mSceneMesh[i + 2];
		tri.CalculateCentroid();

		mSceneTriangles.push_back(tri);
	}
}

KDTreeNode* KDTreeController::BuildSceneTree()
{
	root = BuildTree(mSceneTriangles, 0, true, 0, 0);
	return root;
}

glm::vec3 KDTreeController::Hit(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{
	mHitTrianglesView.clear();

	Ray ray(rayOrigin, rayDirection);

	CalcHitTriangles(root, ray, mHitTriangles);

	if (mHitTriangles.size() > 0)
	{
		std::pair<Triangle, float>& nearestTri = *std::min_element(mHitTriangles.begin(), mHitTriangles.end(), [](const auto& a, const auto& b)
		{
			return a.second < b.second;
		});

		mHitTriangleOne = nearestTri;
		mHitTrianglesView.push_back(nearestTri.first);
		mHitTriangles.clear();

		return mHitTriangleOne.first.GetCentroid();
	}

	return glm::vec3(0, 0, 0);
}

void KDTreeController::DrawBoundingBoxes(KDTreeNode* node, unsigned depth, AssetManager& assetManager)
{
	if (depth == 0) return;

	if (node->leftChild != nullptr)
		DrawBoundingBoxes(node->leftChild, depth - 1, assetManager);

	if (node->rightChild != nullptr)
		DrawBoundingBoxes(node->rightChild, depth - 1, assetManager);

	node->aabb.Draw(assetManager);
}

void KDTreeController::DrawRays(Camera& m_cam)
{
	if (mRayOne != nullptr)
	{
		mRayOne->Draw(m_cam);
	}
}

void KDTreeController::DrawHitTriangles(Camera& m_cam, AssetManager& assetManager)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (OGLTriangle& tri : mHitTrianglesView)
	{
		tri.Draw(glm::vec3(1.0f, 0.0f, 1.0f), m_cam, assetManager);
	}

	glDisable(GL_BLEND);
}

void KDTreeController::PrintRootBoundingBox()
{
	std::cout << "Minimal point: [ " << root->aabb.GetMin().x << " | " << root->aabb.GetMin().y << " | " << root->aabb.GetMin().z << " ]" << std::endl;
	std::cout << "Maximal point: [ " << root->aabb.GetMax().x << " | " << root->aabb.GetMax().y << " | " << root->aabb.GetMax().z << " ]" << std::endl;

	std::cout << "Minimal point left ch: [ " << root->leftChild->aabb.GetMin().x << " | " << root->leftChild->aabb.GetMin().y << " | " << root->leftChild->aabb.GetMin().z << " ]" << std::endl;
	std::cout << "Maximal point left ch: [ " << root->leftChild->aabb.GetMax().x << " | " << root->leftChild->aabb.GetMax().y << " | " << root->leftChild->aabb.GetMax().z << " ]" << std::endl;
}

void KDTreeController::PrintLeafBoundingBoxes(KDTreeNode* node, AssetManager& assetManager)
{
	if (node->leftChild == nullptr && node->rightChild == nullptr)
	{
		node->aabb.Draw(assetManager);
	}
	else
	{
		PrintLeafBoundingBoxes(node->leftChild, assetManager);
		PrintLeafBoundingBoxes(node->rightChild, assetManager);
	}
}

void KDTreeController::RenderAABB(AssetManager& assetManager)
{
	root->aabb.Draw(assetManager);
}

void KDTreeController::GetBoundingBox(std::vector<Triangle>& triangles, glm::vec3& min, glm::vec3& max)
{
	for (GLuint i = 0; i < triangles.size(); ++i)
	{
		if (i == 0)
		{
			min = max = triangles[i].A;
		}

		for (int j = 0; j < 3; ++j)
		{
			glm::vec3 currPt;

			if (j == 0)
				currPt = triangles[i].A;
			else if (j == 1)
				currPt = triangles[i].B;
			else
				currPt = triangles[i].C;

			if (currPt.x > max.x) max.x = currPt.x;
			if (currPt.y > max.y) max.y = currPt.y;
			if (currPt.z > max.z) max.z = currPt.z;
			if (currPt.x < min.x) min.x = currPt.x;
			if (currPt.y < min.y) min.y = currPt.y;
			if (currPt.z < min.z) min.z = currPt.z;
		}
	}
}

KDTreeNode* KDTreeController::BuildTree(std::vector<Triangle> triangleList, int depth, bool maxBBVal, int boundAxis, float val)
{
	KDTreeNode* newNode = new KDTreeNode();
	glm::vec3 minTemp, maxTemp;

	// Are we a leaf node ?
	if (triangleList.size() <= 100)
	{
		// Create the leave node
		GetBoundingBox(triangleList, minTemp, maxTemp);

		newNode->aabb.SetMin(minTemp);
		newNode->aabb.SetMax(maxTemp);
		newNode->aabb.RecalculateBounds();
		newNode->nodeTriangles = triangleList;
		newNode->leftChild = nullptr;
		newNode->rightChild = nullptr;
	}
	else
	{
		// Partition the triangleList and split the space
		glm::vec3 medianPt;
		std::vector<Triangle> leftList;
		std::vector<Triangle> rightList;

		int axis = depth % 3;

		Triangle minX = *std::min_element(triangleList.begin(), triangleList.end(), [&axis](Triangle& a, Triangle& b)
		{
			return a.GetCentroid().x < b.GetCentroid().x;
		});

		Triangle maxX = *std::max_element(triangleList.begin(), triangleList.end(), [&axis](Triangle& a, Triangle& b)
		{
			return a.GetCentroid().x < b.GetCentroid().x;
		});

		Triangle minY = *std::min_element(triangleList.begin(), triangleList.end(), [&axis](Triangle& a, Triangle& b)
		{
			return a.GetCentroid().y < b.GetCentroid().y;
		});

		Triangle maxY = *std::max_element(triangleList.begin(), triangleList.end(), [&axis](Triangle& a, Triangle& b)
		{
			return a.GetCentroid().y < b.GetCentroid().y;
		});

		Triangle minZ = *std::min_element(triangleList.begin(), triangleList.end(), [&axis](Triangle& a, Triangle& b)
		{
			return a.GetCentroid().z < b.GetCentroid().z;
		});

		Triangle maxZ = *std::max_element(triangleList.begin(), triangleList.end(), [&axis](Triangle& a, Triangle& b)
		{
			return a.GetCentroid().z < b.GetCentroid().z;
		});


		float xLength = maxX.GetCentroid().x - minX.GetCentroid().x;
		float yLength = maxY.GetCentroid().y - minY.GetCentroid().y;
		float zLength = maxZ.GetCentroid().z - minZ.GetCentroid().z;
		//std::cout << "X-Length: " << xLength << std::endl;
		//std::cout << "Y-Length: " << yLength << std::endl;
		//std::cout << "Z-Length: " << zLength << std::endl;


		axis = 0;

		if (yLength > xLength)
			axis = 1;

		if (zLength > yLength && zLength > xLength)
			axis = 2;

		/// axis == 0 -> x-axis
		/// axis == 1 -> y-axis
		/// axis == 2 -> z-axis
		std::nth_element(triangleList.begin(), triangleList.begin() + triangleList.size() / 2, triangleList.end(), [&axis](Triangle& a, Triangle& b)
		{
			return a.GetCentroid()[axis] < b.GetCentroid()[axis];
		});

		// Get the median of the triangles
		medianPt = triangleList[triangleList.size() / 2].GetCentroid();

		// Parition the space to create lists of left / right triangles (in relation to the splitting plane)
		for (Triangle tri : triangleList)
		{
			medianPt[axis] >= tri.GetCentroid()[axis] ? leftList.push_back(tri) : rightList.push_back(tri);

			//float maxCoord = std::max({tri.A[axis], tri.B[axis], tri.C[axis]});
			//float minCoord = std::min({tri.A[axis], tri.B[axis], tri.C[axis]});

			//if (maxCoord < medianPt[axis] && minCoord < medianPt[axis])
			//	leftList.push_back(tri);
			//else if (maxCoord > medianPt[axis] && minCoord > medianPt[axis])
			//	rightList.push_back(tri);
			//else
			//{
			//	leftList.push_back(tri);
			//	rightList.push_back(tri);
			//}
		}

		// Create meta-data of the current node (BB, triangleList)
		GetBoundingBox(triangleList, minTemp, maxTemp);

		if (maxBBVal && depth != 0)
		{
			maxTemp[boundAxis] = val;
		}
		else if (!maxBBVal && depth != 0)
		{
			minTemp[boundAxis] = val;
		}

		newNode->aabb.SetMin(minTemp);
		newNode->aabb.SetMax(maxTemp);
		newNode->aabb.RecalculateBounds();
		newNode->nodeTriangles = triangleList;
		// Recursivley create the both subtrees of the current node
		newNode->leftChild = BuildTree(leftList, depth + 1, true, axis, medianPt[axis]);
		newNode->rightChild = BuildTree(rightList, depth + 1, false, axis, medianPt[axis]);
	}

	return newNode;
}

void KDTreeController::CalcHitTriangles(KDTreeNode* node, Ray& ray, std::vector<std::pair<Triangle, float>>& hitTriangles)
{
	/// Check nodes BB if it intersects with the ray
	bool hitBB = RayBoxIntersection(ray, node->aabb);

	/// If yes, check the child-trees of the current node
	if (hitBB)
	{
		node->aabb.mColor = glm::vec3(1.0f, 0.0f, 0.0f);

		if (node->leftChild == nullptr && node->rightChild == nullptr)
		{
			/// We hit a leaf, check all triangles
			// std::cout << "I have " << node->nodeTriangles.size() << " triangles to test!" << std::endl

			for (Triangle tri : node->nodeTriangles)
			{
				float hitDist = 0.0f;
				bool hitTri = RayTriangleIntersection(ray, tri, hitDist);

				if (hitTri)
					hitTriangles.push_back(std::make_pair(tri, hitDist));
			}
		}
		else
		{
			if (node->leftChild != nullptr)
				CalcHitTriangles(node->leftChild, ray, hitTriangles);
			if (node->rightChild != nullptr)
				CalcHitTriangles(node->rightChild, ray, hitTriangles);
		}
	}
}