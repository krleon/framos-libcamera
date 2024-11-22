/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2024, Framos
 *
 * camera helper for imx662 sensor
 */

#include <math.h>

#include "cam_helper.h"

using namespace RPiController;


class CamHelperimx662 : public CamHelper
{
public:
	CamHelperimx662();
	uint32_t gainCode(double gain) const override;
	double gain(uint32_t gainCode) const override;
	void getDelays(int &exposureDelay, int &gainDelay,
		       int &vblankDelay, int &hblankDelay) const override;
	unsigned int hideFramesStartup() const override;
	unsigned int hideFramesModeSwitch() const override;

private:
	/*
	 * Smallest difference between the frame length and integration time,
	 * in units of lines.
	 */
	static constexpr int frameIntegrationDiff = 2;
};

CamHelperimx662::CamHelperimx662()
	: CamHelper({}, frameIntegrationDiff)
{
}

/**
 * TODO: check if this calculation is correct!
 * 
 * FRAMOS added comment:
 * 	-comment source from libcamera/src/ipa/libipa/camera_sensor_helper.cpp
 * 
 * \brief Compute gain code from the analogue gain absolute value
 * \param[in] gain The real gain to pass
 *
 * This function aims to abstract the calculation of the gain letting the IPA
 * use the real gain for its estimations.
 *
 * \return The gain code to pass to V4L2
 */
uint32_t CamHelperimx662::gainCode(double gain) const
{
	/**
	 * max gain in dec. value = 240 -> 0xf0 (analog+digital)
	 * Gain step defined in datasheet is 10/3
	 */
	int code = 20 * log10(gain) * 10/3;
	return std::max(0, std::min(code, 0xf0));
}

/**
 * TODO: check if this calculation is correct!
 * 
 * FRAMOS added comment:
 * 	-comment source from libcamera/src/ipa/libipa/camera_sensor_helper.cpp
 * 
 * \brief Compute the real gain from the V4L2 subdev control gain code
 * \param[in] gainCode The V4L2 subdev control gain
 *
 * This function aims to abstract the calculation of the gain letting the IPA
 * use the real gain for its estimations. It is the counterpart of the function
 * CameraSensorHelper::gainCode.
 *
 * \return The real gain
 */
double CamHelperimx662::gain(uint32_t gainCode) const
{

	/**
	 * uint32_t CamHelperimx662::gainCode(double gain) const
	 * will return 158.680749 for gain value 240
	 * 
	 * We need to convert it back to 240 value
	 */
	return pow(10, 0.015 * gainCode);

}

void CamHelperimx662::getDelays(int &exposureDelay, int &gainDelay,
				int &vblankDelay, int &hblankDelay) const
{
	exposureDelay = 2;
	gainDelay = 2;
	vblankDelay = 2;
	hblankDelay = 2;
}

unsigned int CamHelperimx662::hideFramesStartup() const
{
	/* On startup, we seem to get 1 bad frame. */
	return 1;
}

unsigned int CamHelperimx662::hideFramesModeSwitch() const
{
	/* After a mode switch, we seem to get 1 bad frame. */
	return 1;
}

static CamHelper *create()
{
	return new CamHelperimx662();
}

static RegisterCamHelper reg("fr_imx662", &create);
