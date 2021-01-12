
#include "Camera.h"
#include "ChunkLoader.h"

const float fPlane = 100.0f;
const float nPlane = 0.1f;
const float fov = 90.0f;

const int screenWidth = 1920;
const int screenHeight = 1080;

glm::vec3 Camera::position = glm::vec3(0, 0, 0);
glm::mat4 Camera::projection(1);

Cube* testCube = nullptr;

Camera::Camera()
{
	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	front = glm::vec3(0.0, 0.0, -1.0);
	up = glm::vec3(0.0, 1.0, 0.0);
	view = glm::mat4(1.0f);
	
	for (int i = 0; i < 5; i++)
		frustrumVerts[i] = glm::vec3(0);

	firstMouse = true;

	projection = glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, nPlane, fPlane);
}

void Camera::updateFrustrum()
{
	float fDiffWidth = fPlane * (tan(glm::radians(fov / 2))) * ((float)screenWidth / (float)screenHeight);
	float fDiffHeight = fPlane * (tan(glm::radians(fov / 2)));
	glm::vec3 crossProduct = glm::cross(front, up);
	glm::vec3 vec2DiffWidth = (fDiffWidth * glm::normalize(glm::vec3(crossProduct.x, crossProduct.y, crossProduct.z)));

	glm::vec3 left = glm::normalize(glm::cross(front, glm::vec3(0, -1, 0)));
	glm::vec3 trueUp = glm::normalize(glm::cross(front, left));
	glm::vec3 vec2DiffHeight = (fDiffHeight * glm::vec3(trueUp.x, trueUp.y, trueUp.z));

	frustrumVerts[0] = glm::vec3(position.x, position.y, position.z);
	frustrumVerts[1] = glm::vec3(position.x, position.y, position.z) +
		(fPlane * glm::normalize(glm::vec3(front.x, front.y, front.z))) + vec2DiffWidth + vec2DiffHeight;
	frustrumVerts[2] = glm::vec3(position.x, position.y, position.z) +
		(fPlane * glm::normalize(glm::vec3(front.x, front.y, front.z))) - vec2DiffWidth + vec2DiffHeight;
	frustrumVerts[3] = glm::vec3(position.x, position.y, position.z) +
		(fPlane * glm::normalize(glm::vec3(front.x, front.y, front.z))) + vec2DiffWidth - vec2DiffHeight;
	frustrumVerts[4] = glm::vec3(position.x, position.y, position.z) +
		(fPlane * glm::normalize(glm::vec3(front.x, front.y, front.z))) - vec2DiffWidth - vec2DiffHeight;
}

float triangleArea(glm::vec2 fv[3])
{
	return abs((fv[0].x * (fv[1].y - fv[2].y)) + (fv[1].x * (fv[2].y - fv[0].y)) + (fv[2].x * (fv[0].y - fv[1].y))) / 2.0;
}

bool pointInTriangle(glm::vec2 tri[3], glm::vec2 pt)
{
	glm::vec2 PAB[3] = { pt, tri[0], tri[1] };
	glm::vec2 PBC[3] = { pt, tri[1], tri[2] };
	glm::vec2 PAC[3] = { pt, tri[0], tri[2] };

	float A = triangleArea(tri);
	float A1 = triangleArea(PAB);
	float A2 = triangleArea(PBC);
	float A3 = triangleArea(PAC);

	return round(A1 + A2 + A3) == round(A);
}

bool onSegment(glm::vec2 p, glm::vec2 q, glm::vec2 r)
{
	if (q.x <= std::fmax(p.x, r.x) && q.x >= std::fmin(p.x, r.x) &&
		q.y <= std::fmax(p.y, r.y) && q.y >= std::fmin(p.y, r.y))
		return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(glm::vec2 p, glm::vec2 q, glm::vec2 r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 

	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool lineIntersects(glm::vec2 p1, glm::vec2 q1, glm::vec2 p2, glm::vec2 q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

bool triangleIntersects(glm::vec2 t1[3], glm::vec2 t2[3])
{
	if (lineIntersects(t1[0], t1[1], t2[0], t2[1])) return true;
	if (lineIntersects(t1[0], t1[1], t2[1], t2[2])) return true;
	if (lineIntersects(t1[0], t1[1], t2[0], t2[2])) return true;
	if (lineIntersects(t1[1], t1[2], t2[0], t2[1])) return true;
	if (lineIntersects(t1[1], t1[2], t2[1], t2[2])) return true;
	if (lineIntersects(t1[1], t1[2], t2[0], t2[2])) return true;
	if (lineIntersects(t1[0], t1[2], t2[0], t2[1])) return true;
	if (lineIntersects(t1[0], t1[2], t2[1], t2[2])) return true;
	if (lineIntersects(t1[0], t1[2], t2[0], t2[2])) return true;
	return false;
}

bool Camera::chunkWithinFrustrum(glm::vec2 pt)
{
	glm::vec2 squarePoints[4] = { glm::vec2(pt.x, pt.y), glm::vec2(pt.x + 16, pt.y), glm::vec2(pt.x, pt.y + 16), glm::vec2(pt.x + 16, pt.y + 16) };
	glm::vec2 tri1[3] = { squarePoints[0], squarePoints[1], squarePoints[2] };
	glm::vec2 tri2[3] = { squarePoints[1], squarePoints[2], squarePoints[3] };

	glm::vec2 vec2Tri1[3] = { glm::vec2(frustrumVerts[0].x, frustrumVerts[0].z), glm::vec2(frustrumVerts[1].x, frustrumVerts[1].z), glm::vec2(frustrumVerts[2].x, frustrumVerts[2].z) };
	glm::vec2 vec2Tri2[3] = { glm::vec2(frustrumVerts[0].x, frustrumVerts[0].z), glm::vec2(frustrumVerts[2].x, frustrumVerts[2].z), glm::vec2(frustrumVerts[4].x, frustrumVerts[4].z) };
	glm::vec2 vec2Tri3[3] = { glm::vec2(frustrumVerts[0].x, frustrumVerts[0].z), glm::vec2(frustrumVerts[1].x, frustrumVerts[1].z), glm::vec2(frustrumVerts[3].x, frustrumVerts[3].z) };
	glm::vec2 vec2Tri4[3] = { glm::vec2(frustrumVerts[0].x, frustrumVerts[0].z), glm::vec2(frustrumVerts[3].x, frustrumVerts[3].z), glm::vec2(frustrumVerts[4].x, frustrumVerts[4].z) };

	if (pointInTriangle(vec2Tri1, pt)) return true;
	if (pointInTriangle(vec2Tri2, pt)) return true;
	if (pointInTriangle(vec2Tri3, pt)) return true;
	if (pointInTriangle(vec2Tri4, pt)) return true;

	return triangleIntersects(vec2Tri1, tri1) || triangleIntersects(vec2Tri1, tri2) ||
		triangleIntersects(vec2Tri2, tri1) || triangleIntersects(vec2Tri2, tri2) ||
		triangleIntersects(vec2Tri3, tri1) || triangleIntersects(vec2Tri3, tri2) ||
		triangleIntersects(vec2Tri4, tri1) || triangleIntersects(vec2Tri4, tri2);

	return false;
}

glm::mat4 Camera::getView()
{
	view = glm::mat4(1.0f);
	view = glm::translate(view, position);

	view = glm::lookAt(position, position + front, up);

	return view;
}

glm::vec3 Camera::getPosition()
{
	return position;
}

glm::mat4 Camera::getProjection()
{
	return projection;
}

void Camera::processInput(GLFWwindow* window)
{
	//updateFrustrum();

	float camSpeed = speed * Time::deltaTime();

	int chunkX = floor(position.x / 16);
	int chunkZ = floor(position.z / 16);
	//ChunkLoader::loadChunk(chunkX, chunkZ);
	for (int x = -2; x <= 2; x++)
	{
		for (int z = -2; z <= 2; z++)
		{
			ChunkLoader::loadChunk(chunkX + x, chunkZ + z);
		}
	}
	//ChunkLoader::loadChunk(chunkX, chunkZ);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += camSpeed * front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= camSpeed * front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= glm::normalize(glm::cross(front, up)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += glm::normalize(glm::cross(front, up)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += glm::vec3(0, speed * Time::deltaTime(), 0);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed = 40.0f;
	else
		speed = 10.0f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		int x = ((int)floor(position.x)) % chunkSize;
		int y = ((int)floor(position.y)) % chunkHeight;
		int z = ((int)floor(position.z)) % chunkSize;
		ChunkLoader::destroyBlock(chunkX, chunkZ, x, y, z);
	}
}

void Camera::mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = yPos - lastY;
	lastX = xPos;
	lastY = yPos;

	xOffset *= sensitivity;
	yOffset *= sensitivity;

	rotation += glm::vec3(-yOffset, xOffset, 0);

	if (rotation.x > 89.0f)
		rotation.x = 89.0f;
	if (rotation.x < -89.0f)
		rotation.x = -89.0f;

	glm::vec3 newFront;
	newFront.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	newFront.y = sin(glm::radians(rotation.x));
	newFront.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	front = glm::normalize(newFront);
}

glm::mat4 Camera::getRotationMatrix()
{
	glm::quat quatRotation = glm::quat(rotation);
	glm::mat4 rotationMatrix(quatRotation);
	return rotationMatrix;
}