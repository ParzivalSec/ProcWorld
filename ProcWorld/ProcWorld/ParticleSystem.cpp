#include "ParticleSystem.h"
#include "ShaderProgram.h"
#include "AssetManager.h"
#include <gtc/type_ptr.hpp>
#include "OpenGLRenderer.h"

Particles::System::System(glm::vec3 emitterPosition, float emitterTTL)
	: m_emitter(emitterPosition, emitterTTL, 0)
	, m_VBO{0, 0}
	, m_VAO{0, 0}
	, m_renderShader(0)
	, m_updateShader(0)
	, m_query(0)
	, m_particleCount(1)
	, m_smokeFrequency(1.0f)
	, m_smokeAccumulator(0.0f)
	, m_smokeTTL(3.0f)
	, m_rocketFrequencyMin(2.0f)
	, m_rocketFrequencyMax(4.0f)
	, m_rocketAccumulator(0.0f)
	, m_rocketTTL(2.0f)
	, m_rocketAmount(2.0f)
	, m_fireworkAmount(5.0f)
	, m_fireworkTTL(1.5f)
	, m_currentBuffer(0)
{
	SetupBuffers();

	// Push emitter into the first buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle), &m_emitter);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Particles::System::SetupBuffers(void)
{
	glGenBuffers(2, m_VBO);
	glGenVertexArrays(2, m_VAO);
	glGenQueries(1, &m_query);

	for (size_t i = 0; i < 2; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[i]);
			glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(Particle), nullptr, GL_STREAM_COPY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(m_VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[i]);
		glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
		glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
			glVertexAttribIPointer(3, 1, GL_INT, sizeof(Particle), reinterpret_cast<GLvoid*>(7 * sizeof(float)));
		glBindVertexArray(0);
	}
}

void Particles::System::SetupRenderShader(AssetManager& assetManager)
{
	ShaderProgram& renderPrg = assetManager.AddShaderSet("Particle_Render_Shader");
	renderPrg.AddShaders(GL_VERTEX_SHADER, "RenderParticles.vert", GL_FRAGMENT_SHADER, "RenderParticles.frag");

	m_renderShader = renderPrg.m_id;

	renderPrg.Link();
}

void Particles::System::SetupUpdateShader(AssetManager& assetManager)
{
	ShaderProgram& updatePrg = assetManager.AddShaderSet("Particle_Update_Shader");
	updatePrg.AddShaders(GL_VERTEX_SHADER, "UpdateParticles.vert", GL_GEOMETRY_SHADER, "UpdateParticles.geom");

	m_updateShader = updatePrg.m_id;

	const GLchar* feedbackVaryings[] = { "outPosition", "outVelocity", "outLifeTime", "outType" };
	glTransformFeedbackVaryings(m_updateShader, 4, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	updatePrg.Link();
}

void Particles::System::Tick(float deltaTime)
{
	m_smokeAccumulator >= m_smokeFrequency ? m_smokeAccumulator = 0.0f : m_smokeAccumulator += deltaTime;
	m_rocketAccumulator >= m_rocketFrequencyMax ? m_rocketAccumulator = 0.0f : m_rocketAccumulator += deltaTime;

	glEnable(GL_RASTERIZER_DISCARD);

	glUseProgram(m_updateShader);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBO[1 - m_currentBuffer]);

	// Smoke uniforms
	GLuint attribLoc = glGetUniformLocation(m_updateShader, "SmokeFrequency");
	glUniform1f(attribLoc, m_smokeFrequency);

	attribLoc = glGetUniformLocation(m_updateShader, "SmokeAccumulator");
	glUniform1f(attribLoc, m_smokeAccumulator);

	attribLoc = glGetUniformLocation(m_updateShader, "SmokeLifetimeInitial");
	glUniform1f(attribLoc, m_smokeTTL);

	// Rocket unigforms
	attribLoc = glGetUniformLocation(m_updateShader, "RocketFrequencyMax");
	glUniform1f(attribLoc, m_rocketFrequencyMax);

	attribLoc = glGetUniformLocation(m_updateShader, "RocketAccumulator");
	glUniform1f(attribLoc, m_rocketAccumulator);

	attribLoc = glGetUniformLocation(m_updateShader, "RocketLifetimeInitial");
	glUniform1f(attribLoc, m_rocketTTL);

	attribLoc = glGetUniformLocation(m_updateShader, "RocketAmount");
	glUniform1i(attribLoc, m_rocketAmount);

	// Firework uniforms
	attribLoc = glGetUniformLocation(m_updateShader, "FireworkAmount");
	glUniform1i(attribLoc, m_fireworkAmount);

	attribLoc = glGetUniformLocation(m_updateShader, "FireworkLifetimeInitial");
	glUniform1f(attribLoc, m_fireworkTTL);

	// General Uniforms
	attribLoc = glGetUniformLocation(m_updateShader, "DeltaTime");
	glUniform1f(attribLoc, deltaTime);

	OpenGLRenderer::BindVertexArray(m_VAO[m_currentBuffer]);
		// Begin: Transform Feedback
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_query);
		glBeginTransformFeedback(GL_POINTS);
			glDrawArrays(GL_POINTS, 0, m_particleCount);
		// End: Tranform Feedback
		glEndTransformFeedback();
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	OpenGLRenderer::UnbindVertexArray();
	glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_particleCount);


	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glFlush();
	glDisable(GL_RASTERIZER_DISCARD);
}

void Particles::System::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	GLuint uniformLocation;

	glUseProgram(m_renderShader);
	uniformLocation = glGetUniformLocation(m_renderShader, "model");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(model));
	uniformLocation = glGetUniformLocation(m_renderShader, "view");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(view));
	uniformLocation = glGetUniformLocation(m_renderShader, "projection");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(m_VAO[m_currentBuffer]);
		glDrawArrays(GL_POINTS, 0, m_particleCount);
	glBindVertexArray(0);

	m_currentBuffer = 1 - m_currentBuffer;
}
