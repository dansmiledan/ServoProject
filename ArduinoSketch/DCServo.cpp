#include "DCServo.h"

DCServo* DCServo::getInstance()
{
    static DCServo dcServo;
    return &dcServo;
}

DCServo::DCServo() :
        controlEnabled(false),
        onlyUseMainEncoderControl(false),
        openLoopControlMode(false),
        pwmOpenLoopMode(false),
        loopNumber(0),
        current(0),
        controlSignal(0),
        uLimitDiff(0),
        Ivel(0),
        mainEncoderHandler(ConfigHolder::createMainEncoderHandler()),
        outputEncoderHandler(ConfigHolder::createOutputEncoderHandler()),
        kalmanFilter(std::make_unique<KalmanFilter>())
{
    threads.push_back(createThread(2, 1200, 0,
        [&]()
        {
            mainEncoderHandler->triggerSample();
            if (outputEncoderHandler)
            {
                outputEncoderHandler->triggerSample();
            }
        }));

    refInterpolator.setGetTimeInterval(1200);
    refInterpolator.setLoadTimeInterval(12000);

    currentController = ConfigHolder::createCurrentController();

    mainEncoderHandler->init();
    if (outputEncoderHandler)
    {
        outputEncoderHandler->init();
    }

#ifdef SIMULATE
    rawMainPos = 2048;
    rawOutputPos = rawMainPos;
#else
    mainEncoderHandler->triggerSample();
    if (outputEncoderHandler)
    {
        outputEncoderHandler->triggerSample();
    }

    rawMainPos = mainEncoderHandler->getValue() * ConfigHolder::getMainEncoderGearRation();
    if (outputEncoderHandler)
    {
        rawOutputPos = outputEncoderHandler->getValue();
    }
    else
    {
        rawOutputPos = rawMainPos;
    }
#endif

    initialOutputPosOffset = rawOutputPos - rawMainPos;
    outputPosOffset = initialOutputPosOffset;

    x[0] = rawMainPos;
    x[1] = 0;
    x[2] = 0;

#ifdef SIMULATE
    xSim = x;
#endif

    kalmanFilter->reset(x);

    loadNewReference(x[0], 0, 0);

    threads.push_back(createThread(1, 1200, 0,
        [&]()
        {
            controlLoop();
            loopNumber++;
        }));
}

bool DCServo::isEnabled()
{
    ThreadInterruptBlocker blocker;
    return controlEnabled;
}

void DCServo::enable(bool b)
{
    ThreadInterruptBlocker blocker;
    if (!isEnabled() && b)
    {
        L << ConfigHolder::ControlParameters::getLVector(controlSpeed), backlashControlSpeed, backlashControlSpeedVelGain, backlashSize;
    }

    controlEnabled = b;

    if (!b)
    {
        refInterpolator.resetTiming();
    }
}

void DCServo::openLoopMode(bool enable, bool pwmMode)
{
    ThreadInterruptBlocker blocker;
    openLoopControlMode = enable;
    pwmOpenLoopMode = pwmMode;
}

void DCServo::onlyUseMainEncoder(bool b)
{
    ThreadInterruptBlocker blocker;
    onlyUseMainEncoderControl = b;
}

void DCServo::setControlSpeed(uint8_t controlSpeed)
{
    this->controlSpeed = controlSpeed;
}

void DCServo::setBacklashControlSpeed(uint8_t backlashControlSpeed, uint8_t backlashControlSpeedVelGain, uint8_t backlashSize)
{
    this->backlashControlSpeed = backlashControlSpeed;
    this->backlashControlSpeedVelGain = backlashControlSpeedVelGain;
    this->backlashSize = backlashSize;
}

void DCServo::loadNewReference(float pos, int16_t vel, int16_t feedForwardU)
{
    ThreadInterruptBlocker blocker;
    refInterpolator.loadNew(pos, vel, feedForwardU);
}

void DCServo::triggerReferenceTiming()
{
    ThreadInterruptBlocker blocker;
    refInterpolator.updateTiming();
}

float DCServo::getPosition()
{
    ThreadInterruptBlocker blocker;
    return rawOutputPos;
}

int16_t DCServo::getVelocity()
{
    ThreadInterruptBlocker blocker;
    return x[1];
}

int16_t DCServo::getControlSignal()
{
    ThreadInterruptBlocker blocker;
    if (controlEnabled)
    {
        return controlSignal;
    }
    return 0;
}

int16_t DCServo::getCurrent()
{
    ThreadInterruptBlocker blocker;
    return current;
}

int16_t DCServo::getPwmControlSignal()
{
    ThreadInterruptBlocker blocker;
    return pwmControlSIgnal;
}

uint16_t DCServo::getLoopNumber()
{
    ThreadInterruptBlocker blocker;
    return loopNumber;
}

float DCServo::getBacklashCompensation()
{
    ThreadInterruptBlocker blocker;
    return outputPosOffset - initialOutputPosOffset;
}

float DCServo::getMainEncoderPosition()
{
    ThreadInterruptBlocker blocker;
    return rawMainPos + initialOutputPosOffset;
}

template <class T>
OpticalEncoderHandler::DiagnosticData getMainEncoderRawDiagnosticDataDispatch(T& encoder)
{
    OpticalEncoderHandler::DiagnosticData out = {0};
    return out;
}

template <>
OpticalEncoderHandler::DiagnosticData getMainEncoderRawDiagnosticDataDispatch(std::unique_ptr<OpticalEncoderHandler>& encoder)
{
    return encoder->getDiagnosticData();
}

template <>
OpticalEncoderHandler::DiagnosticData DCServo::getMainEncoderDiagnosticData()
{
    ThreadInterruptBlocker blocker;
    return getMainEncoderRawDiagnosticDataDispatch(mainEncoderHandler);
}

void DCServo::controlLoop()
{
#ifdef SIMULATE
    xSim = kalmanFilter->getA() * xSim + kalmanFilter->getB() * controlSignal;
    rawMainPos =xSim[0];
    rawOutputPos = rawMainPos;
#else
    rawMainPos = mainEncoderHandler->getValue() * ConfigHolder::getMainEncoderGearRation();
    if (outputEncoderHandler)
    {
        rawOutputPos = outputEncoderHandler->getValue();
    }
    else
    {
        rawOutputPos = rawMainPos;
    }
#endif

    x = kalmanFilter->update(controlSignal, rawMainPos);

    if (controlEnabled)
    {
        if (!openLoopControlMode)
        {
            uLimitDiff = 0.99 * uLimitDiff + 0.01 * (controlSignal - currentController->getLimitedRef());

            Ivel += L[3] * uLimitDiff;

            float posRef;
            float velRef;
            float feedForwardU;

            refInterpolator.getNext(posRef, velRef, feedForwardU);

            if (!onlyUseMainEncoderControl)
            {
                int newForceDir = 0;
                if (feedForwardU > 1.0)
                {
                    newForceDir = 1;
                }
                else if (feedForwardU < -1.0)
                {
                    newForceDir = -1;
                }

                if (newForceDir != 0)
                {
                    if (newForceDir != forceDir && forceDir != 0)
                    {
                        outputPosOffset -= newForceDir * currentBacklashStepSize;
                    }
                    forceDir = newForceDir;
                    lastForceDirNotZero = true;
                }
                else if (lastForceDirNotZero)
                {
                    lastForceDirNotZero = false;
                    currentBacklashStepSize = L[6];
                }
                
                double gain = L[4] * (0.1 + 0.9 * std::max(0.0,
                        1.0 - L[5] * (1.0 / 255) * (1.0 / 10) * std::abs(velRef)));
                double backlashCompensationDiff = gain * 0.0012 * (posRef - rawOutputPos);
                outputPosOffset -= backlashCompensationDiff;
                currentBacklashStepSize = std::max(0.0,
                        currentBacklashStepSize - std::abs(backlashCompensationDiff));
            }

            posRef -= outputPosOffset;

            float posDiff = posRef - x[0];

            float vControlRef = L[0] * posDiff + velRef;

            float u = L[1] * (vControlRef - x[1]) + Ivel + feedForwardU;

            controlSignal = u;

            currentController->updateVelocity(x[1]);
            currentController->setReference(static_cast<int16_t>(controlSignal));
            currentController->applyChanges();
            current = currentController->getCurrent();
            pwmControlSIgnal = currentController->getFilteredPwm();

            Ivel -= L[2] * (vControlRef - x[1]);
        }
        else
        {
            Ivel = 0;
            uLimitDiff = 0;
            outputPosOffset = rawOutputPos - rawMainPos;

            float posRef;
            float velRef;
            float feedForwardU;

            refInterpolator.getNext(posRef, velRef, feedForwardU);

            if (pwmOpenLoopMode)
            {
                controlSignal = 0;
                currentController->overidePwmDuty(feedForwardU);
            }
            else
            {
                controlSignal = feedForwardU;
                currentController->setReference(static_cast<int16_t>(controlSignal));
            }
            currentController->applyChanges();
            current = currentController->getCurrent();
            pwmControlSIgnal = currentController->getFilteredPwm();
        }
    }
    else
    {
        loadNewReference(x[0], 0, 0);
        Ivel = 0;
        uLimitDiff = 0;
        outputPosOffset = rawOutputPos - rawMainPos;
        controlSignal = 0;
        currentController->activateBrake();
        currentController->applyChanges();
        current = currentController->getCurrent();
        pwmControlSIgnal = currentController->getFilteredPwm();
    }

}

ReferenceInterpolator::ReferenceInterpolator()
{
}

void ReferenceInterpolator::loadNew(float position, float velocity, float feedForward)
{
    if (timingInvalid)
    {
        midPointTimeOffset = 0;

        pos[2] = position;
        vel[2] = velocity;
        feed[2] = feedForward;

        pos[1] = pos[2];
        vel[1] = vel[2];
        feed[1] = feed[2];

        pos[0] = pos[1];
        vel[0] = vel[1];
        feed[0] = feed[1];
    }
    else
    {
        if (midPointTimeOffset > -loadTimeInterval)
        {
            midPointTimeOffset -= loadTimeInterval;
        }

        pos[0] = pos[1];
        vel[0] = vel[1];
        feed[0] = feed[1];

        pos[1] = pos[2];
        vel[1] = vel[2];
        feed[1] = feed[2];

        pos[2] = position;
        vel[2] = velocity;
        feed[2] = feedForward;
    }
}

void ReferenceInterpolator::updateTiming()
{
    uint16_t timestamp = micros();
    if (timingInvalid)
    {
        midPointTimeOffset = getTimeInterval;

        timingInvalid = false;
    }
    else
    {
        uint16_t updatePeriod = timestamp - lastUpdateTimingTimestamp;
        uint16_t timeSinceLastGet = timestamp - lastGetTimestamp;

        int16_t timingError = getTimeInterval - (midPointTimeOffset + timeSinceLastGet);
        int16_t periodError = updatePeriod - loadTimeInterval;

        midPointTimeOffset += timingError / 8;
        loadTimeInterval += periodError / 16;

        invertedLoadInterval = 1.0 / loadTimeInterval;
    }

    lastUpdateTimingTimestamp = timestamp;
}

void ReferenceInterpolator::resetTiming()
{
    timingInvalid = true;
}

void ReferenceInterpolator::getNext(float& position, float& velocity, float& feedForward)
{
    lastGetTimestamp = micros();

    if (midPointTimeOffset < 2 * loadTimeInterval)
    {
        midPointTimeOffset += getTimeInterval;
    }

    float t = midPointTimeOffset * invertedLoadInterval;

    if (t < -1.0)
    {
        t = -1.0;
    }
    else if (t > 1.2)
    {
        t = 1.2;
    }

    if (t < 0.0)
    {
        t += 1.0;
        position = pos[0] + t * (pos[1] - pos[0]);
        velocity = vel[0] + t * (vel[1] - vel[0]);
        feedForward = feed[0] + t * (feed[1] - feed[0]);
    }
    else
    {
        position = pos[1] + t * (pos[2] - pos[1]);
        velocity = vel[1] + t * (vel[2] - vel[1]);
        feedForward = feed[1] + t * (feed[2] - feed[1]);
    }
}

void ReferenceInterpolator::setGetTimeInterval(const uint16_t& interval)
{
    timingInvalid = true;

    midPointTimeOffset = 0;
    getTimeInterval = interval;
}

void ReferenceInterpolator::setLoadTimeInterval(const uint16_t& interval)
{
    timingInvalid = true;
    midPointTimeOffset = 0;

    loadTimeInterval = interval;
    invertedLoadInterval = 1.0 / loadTimeInterval;
}
