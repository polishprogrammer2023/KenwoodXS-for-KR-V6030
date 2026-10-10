// SPDX-License-Identifier: Apache-2.0

#ifndef KenwoodXS_H__
#define KenwoodXS_H__

#include <Arduino.h>

// Kenwood XS System Control Commands
enum KenwoodXSCommand {
    // Power & Miscellaneous
    POWER_ON     = 0xB5, // works
    POWER_OFF    = 0xB9, // works
    BAND         = 0xBE, // works
    PLUS_10      = 0xA4, // idk how to test it

    // Input Selection
    TAPE_2_ON    = 0x14, // same status as phono
    TAPE_2_OFF   = 0x18, // same status as phono
    VIDEO_2      = 0x71, // same as phono
    PHONO        = 0x73, // unknown, KR-6030 sends it, but when sending it to it does nothing
    TAPE_1       = 0x5E, // works
    VIDEO_1      = 0x7A, // same as phono
    TUNER        = 0xBE, // same as band
    CD            = 0xDE, // works

    // Tape A Controls
    TA_FAST_FORWARD = 0x43, // don't want to setup my KX-W4080
    TA_STOP         = 0x45, // don't want to setup my KX-W4080
    TA_PLAY         = 0x47, // don't want to setup my KX-W4080
    TA_RECORD       = 0x49, // don't want to setup my KX-W4080
    TA_REWIND       = 0x4B, // don't want to setup my KX-W4080
    TA_PAUSE        = 0x4D, // don't want to setup my KX-W4080
    TA_REWIND_SLOW  = 0x4F, // don't want to setup my KX-W4080

    // Tape B Controls
    TB_FAST_FORWARD = 0x42, // don't want to setup my KX-W4080
    TB_STOP         = 0x44, // don't want to setup my KX-W4080
    TB_PLAY         = 0x46, // don't want to setup my KX-W4080
    TB_RECORD       = 0x48, // don't want to setup my KX-W4080
    TB_REWIND       = 0x4A, // don't want to setup my KX-W4080
    TB_PAUSE        = 0x4C, // don't want to setup my KX-W4080
    TB_REWIND_SLOW  = 0x4E, // don't want to setup my KX-W4080

    // CD Controls
    CD_PLAY_PAUSE   = 0xC2, // don't have a XS CD player
    CD_STOP         = 0xC6, // don't have a XS CD player
    CD_DISC         = 0xAE, // don't have a XS CD player
    CD_FAST_FORWARD = 0xA1, // don't have a XS CD player
    CD_REWIND       = 0xA9, // don't have a XS CD player
    CD_NEXT_TRACK   = 0xC0, // don't have a XS CD player
    CD_PREV_TRACK   = 0xC8, // don't have a XS CD player
    CD_SCRUB_STOP   = 0xB1, // don't have a XS CD player
    CD_START        = 0x7D  // don't have a XS CD player
};

enum KenwoodXSMode {
    KENWOOD_XS_TRANSMIT_ONLY,
    KENWOOD_XS_BIDIRECTIONAL
};

class KenwoodXS {

public:
typedef void (*OnCommandSentCallback)(byte command);
typedef void (*OnCommandReceivedCallback)(byte command);

    KenwoodXS(int ctrlPin, int sdatPin);
    void begin(KenwoodXSMode mode = KENWOOD_XS_TRANSMIT_ONLY);
    void begin(bool bidirectional);
    void setMode(KenwoodXSMode mode);
    KenwoodXSMode mode() const;
    void sendCommand(byte command);
    void sendCommand(KenwoodXSCommand command);
    void poll();
    bool available();
    bool readCommand(byte &command);
    bool readCommand(byte &command, unsigned long timeoutMs);
    void powerOn();
    void powerOff();
    void selectInput(KenwoodXSCommand input);
    void playTapeA();
    void stopTapeA();
    void playTapeB();
    void stopTapeB();
    void playCD();
    void stopCD();
    void startCD();
    void tryAllCommands(unsigned long delayMs = 1000);
    void onCommandSentCallback(OnCommandSentCallback callback);
    void onCommandReceivedCallback(OnCommandReceivedCallback callback);
    void setDebugOutput(bool enabled);

private:
    enum ReceiveState {
        RX_IDLE,
        RX_WAIT_START_RISE,
        RX_WAIT_START_FALL,
        RX_WAIT_BIT_RISE,
        RX_WAIT_BIT_FALL,
        RX_WAIT_CTRL_LOW
    };

    void sendByte(byte b);
    void driveIdle();
    void releaseBus();
    void resetReceiveState();
    void finishReceivedCommand(byte command);
    bool classifyPulse(unsigned long widthUs, bool &bitValue) const;
    int _ctrlPin;
    int _sdatPin;
    KenwoodXSMode _mode;
    bool _debugOutput;
    bool _hasReceivedCommand;
    byte _lastReceivedCommand;
    ReceiveState _rxState;
    byte _rxCommand;
    byte _rxBitCount;
    unsigned long _rxStateStartedAt;
    unsigned long _rxLowStartedAt;
    OnCommandSentCallback _onCommandSentCallback;
    OnCommandReceivedCallback _onCommandReceivedCallback;
    
    // Protocol timing constants
    static const int BIT_0 = 15000;  // 15ms for bit 0
    static const int BIT_1 = 7500;   // 7.5ms for bit 1
    static const int START_BIT_DELAY = 15;  // 15ms start bit
    static const int CTRL_SETUP_DELAY = 2;  // 2ms CTRL setup time
    static const int FRAME_SIGNAL_DELAY = 7500;  // 7.5ms frame signal
    static const unsigned long RX_ONE_ZERO_THRESHOLD_US = 7240; // fixed timings, because it showed recived command 0xff
    static const unsigned long RX_MIN_PULSE_US = 3000; // fixed timings, because it showed recived command 0xff
    static const unsigned long RX_MAX_PULSE_US = 13000; // fixed timings, because it showed recived command 0xff
    static const unsigned long RX_START_TIMEOUT_US = 20000; // fixed timings, because it showed recived command 0xff
    static const unsigned long RX_FRAME_TIMEOUT_US = 20000; // fixed timings, because it showed recived command 0xff
};

#endif // KenwoodXS_H__
