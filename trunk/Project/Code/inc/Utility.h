#pragma once

#ifdef _DEBUG
#define CHECK_GL_ERROR() Utility::CheckGLError(__FILE__, __LINE__)
#else
#define CHECK_GL_ERROR()
#endif

class Utility
{
public:

	/**
	 * @param v
	 * @return the log base 2 of the argument
	 */
	static inline float log2(const float& v)
	{
		return log(v) / log(2.0f);
	}

	/**
	 * @param v the value to be clamped
	 * @param lowerThreshold the lower clamping threshold
	 * @param upperThreshold the upper clamping threshold
	 */
	static inline void clamp(float& v, const float& lowerThreshold, const float& upperThreshold)
	{
		if (v < lowerThreshold)
		{
			v = lowerThreshold;
		}
		else if ( v > upperThreshold)
		{
			v = upperThreshold;
		}
	}

	// GL ERROR CHECK
	static inline int CheckGLError(char *aFile, int aLine)
	{
		int result = 0;
		GLenum glError = glGetError();

		while (glError != GL_NO_ERROR)
		{
			cout << "Error in file " << aFile << " at line: " << aLine << endl;
			cout << "GL Error #" << glError << ": " << gluErrorString(glError) << endl << endl;
			result = 1;
			glError = glGetError();
		}

		return result;
	}
};