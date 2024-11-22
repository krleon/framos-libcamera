/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2024, Framos
 *
 * camera helper for imx900 sensor
 */

#include <math.h>

#include "cam_helper.h"

using namespace RPiController;


class CamHelperimx900 : public CamHelper
{
public:
	CamHelperimx900();
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

CamHelperimx900::CamHelperimx900()
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
uint32_t CamHelperimx900::gainCode(double gain) const
{
	/**
	 * max gain in dec. value = 480 -> 0x1E0 (analog+digital)
	 * Gain step defined in datasheet is 0.1
	 */
	int code = 20 * log10(gain) * 10;
	return std::max(0, std::min(code, 0x1E0));
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
double CamHelperimx900::gain(uint32_t gainCode) const
{

	/**
	 * uint32_t CamHelperimx900::gainCode(double gain) const
	 * will return 536.24824747 for gain value 480
	 * 
	 * We need to convert it back to 480 value
	 */
	return pow(10, 0.005 * gainCode);

}

void CamHelperimx900::getDelays(int &exposureDelay, int &gainDelay,
				int &vblankDelay, int &hblankDelay) const
{
	exposureDelay = 2;
	gainDelay = 2;
	vblankDelay = 2;
	hblankDelay = 2;
}

unsigned int CamHelperimx900::hideFramesStartup() const
{
	/* On startup, we seem to get 1 bad frame. */
	return 1;
}

unsigned int CamHelperimx900::hideFramesModeSwitch() const
{
	/* After a mode switch, we seem to get 1 bad frame. */
	return 1;
}

static CamHelper *create()
{
	return new CamHelperimx900();
}

static RegisterCamHelper reg("fr_imx900", &create);
