#pragma once

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

};