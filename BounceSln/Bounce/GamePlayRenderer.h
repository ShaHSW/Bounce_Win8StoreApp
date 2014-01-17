#pragma once

#include "DirectXBase.h"
#include "BasicLoader.h"
#include "AudioData.h"

#define PADDLE_LENGTH_RELATIVE         0.15f
#define PADDLE_LENGTH                  75.0f
#define PADDLE_WIDTH                   15.0f
#define DEFAULT_PADDLE_MOVEMENT_SPEED  0.02f
#define CONTROL_SPACE_WIDTH            90.0f
#define TABLE_BORDER_HEIGHT            70.0f
#define DISPLAY_MARGIN                 10.0f

#define BALL_RADIUS            20.0f
#define BALL_INIT_BUFFER       50.0f

#define MAX_BALL_BITMAPS       2
#define BALL_TRAIL             5

#define BALL_BITMAP_1          L"Assets\\blue_ball.png"
#define BALL_BITMAP_2          L"Assets\\red_ball.png"

#define TABLE_BITMAP           L"Assets\\table.png"
#define PADDLE_BITMAP          L"Assets\\paddle.png"

#define COUNT_DOWN_3           L"Assets\\cd_3.png"
#define COUNT_DOWN_2           L"Assets\\cd_2.png"
#define COUNT_DOWN_1           L"Assets\\cd_1.png"
#define COUNT_DOWN_GO          L"Assets\\cd_go.png"
#define COUNT_DOWN_SOUND       L"Assets\\countdown.wav"

#define GAMESTART_MUSIC        L"Assets\\gamestart.wav"
#define GAMEOVER_BITMAP        L"Assets\\gameover.png"
#define GAMEOVER_MUSIC         L"Assets\\gameover.wav"

#define BOUNCE_WALL_SOUND      L"Assets\\wallbounce.wav"
#define NEW_BALL_SOUND         L"Assets\\newball.wav"
#define GOOD_BALL_HIT_SOUND    L"Assets\\goodhit.wav"
#define BAD_BALL_HIT_SOUND     L"Assets\\badhit.wav"

#define TOTAL_DIGITS           10
#define SCORE_DISPLAY_DIGITS   9
#define LIVES_DISPLAY_DIGITS   1
#define DIGIT_BITMAPS_0        L"Assets\\score_0.png"
#define DIGIT_BITMAPS_1        L"Assets\\score_1.png"
#define DIGIT_BITMAPS_2        L"Assets\\score_2.png"
#define DIGIT_BITMAPS_3        L"Assets\\score_3.png"
#define DIGIT_BITMAPS_4        L"Assets\\score_4.png"
#define DIGIT_BITMAPS_5        L"Assets\\score_5.png"
#define DIGIT_BITMAPS_6        L"Assets\\score_6.png"
#define DIGIT_BITMAPS_7        L"Assets\\score_7.png"
#define DIGIT_BITMAPS_8        L"Assets\\score_8.png"
#define DIGIT_BITMAPS_9        L"Assets\\score_9.png"

#define TOTAL_DIGITS_BLUE      4
#define DIGIT_BITMAPS_BLUE_0   L"Assets\\count_0.png"
#define DIGIT_BITMAPS_BLUE_1   L"Assets\\count_1.png"
#define DIGIT_BITMAPS_BLUE_2   L"Assets\\count_2.png"
#define DIGIT_BITMAPS_BLUE_3   L"Assets\\count_3.png"

#define WAIT_TIME              4
#define GAME_END_WAIT_TIME     1
#define GAME_SCORE_WAIT_TIME   2
class GamePlayers
{
public:
    static const int PLAYER_1 = 0;
    static const int PLAYER_2 = 1;
};
#define MAX_PLAYERS 2
#define MIN_PLAYERS 2

class BallType
{
public:
    static const int BALL_GOOD          = 0;
    static const int BALL_BAD           = 1;
    static const int BALL_NEUTRAL       = 2;
};

#define MAX_BALL_SHAPES      2
#define MAX_BALLS            3
#define MIN_BALLS            1
#define MAX_BALL_SPEED       10.0f
#define MIN_BALL_SPEED       1.0f
#define DEFAULT_BALL_SPEED   2.0f
#define BALL_SPEED_DELTA     0.1f
#define MAX_BALLS_MISSED     3

class GamePlayScreen
{
public:
    static const int DEFAULT_SCREEN      = 0;
    static const int COUNT_DOWN_SCREEN   = 1;
    static const int GAME_PLAY_SCREEN    = 2;
    static const int GAME_OVER_SCREEN    = 3;
    static const int GAME_SCORE_SCREEN   = 4;
};



class GamePlayDifficulty
{
public:
    static const int GAME_VERY_EASY = 0;
    static const int GAME_EASY      = 1;
    static const int GAME_NORMAL    = 2;
    static const int GAME_HARD      = 3;
    static const int GAME_NO_WAY    = 4;
};

// This class renders simple text with a colored background.
ref class GamePlayRenderer sealed : public DirectXBase
{
public:
	GamePlayRenderer();

	// DirectXBase methods.
	virtual void CreateDeviceIndependentResources() override;
	virtual void CreateDeviceResources() override;
	virtual void CreateWindowSizeDependentResources() override;
	virtual void Render() override;

    void InitializeGame();

	// Method for updating time-dependent objects.
	void Update(float timeTotal, float timeDelta);
    void UpdatePaddleMovementDelta(int player, float delta);

    event Windows::Foundation::EventHandler<Platform::Object^>^ GameOver;

    void PlayCountDown(){m_audioCountdown->Play(1.0f);}
    void PlayGameStart(){m_musicGameStart->Play(1.0f);}
    void PlayGameOver(){m_musicGameOver->Play(1.0f);}
    void PlayNewBall(){m_audioNewBall->Play(1.0f);}
    void PlayBounce(){m_audioBounce->Play(1.0f);}
    void PlayGoodHit(){m_audioGoodHit->Play(1.0f);}
    void PlayBadHit(){m_audioBadHit->Play(1.0f);}

    void SetGamePlayScreen(int screen)
    {
        switch (screen)
        {
        case GamePlayScreen::DEFAULT_SCREEN:
        case GamePlayScreen::COUNT_DOWN_SCREEN:
        case GamePlayScreen::GAME_PLAY_SCREEN:
        case GamePlayScreen::GAME_OVER_SCREEN:
        case GamePlayScreen::GAME_SCORE_SCREEN:
            m_gamePlayScreen = screen;
            break;
        }
    }
    int GetGamePlayScreen(){ return m_gamePlayScreen; }

    void SetGamePlayDifficulty(int difficulty)
    {
        switch (m_gamePlayDifficulty)
        {
        case GamePlayDifficulty::GAME_VERY_EASY:
        case GamePlayDifficulty::GAME_EASY:
        case GamePlayDifficulty::GAME_NORMAL:
        case GamePlayDifficulty::GAME_HARD:
        case GamePlayDifficulty::GAME_NO_WAY:
            m_gamePlayDifficulty = difficulty;
            break;
        }
    }
    int GetGamePlayDifficulty(){ return m_gamePlayDifficulty;}

    void RenderNeeded() { m_renderNeeded = true;}
    int GetRenderState(){ return m_renderNeeded ;}

    void ResetDurationTimer() { m_gameDuration = 0;}
    int IncementDurationCount(){ return ++m_gameDuration;}
    int GetDuration() {return m_gameDuration;}

    void UpdateBallVelocities();

	// Methods to save and load state in response to suspend.
	void SaveInternalState(Windows::Foundation::Collections::IPropertySet^ state);
	void LoadInternalState(Windows::Foundation::Collections::IPropertySet^ state);

    void SuspendAudio();
    void ResumeAudio();

private:
    void RenderDefaultScreen(void);
    void RenderCountDownScreen(void);
    void RenderGamePlayAgainstScreen(void);
    void RenderGameOverScreen(void);
    void RenderGameScoreScreen(void);
    
    void UpdatePaddlePosition(int player, float delta);
    void UpdateAllPaddles();

    void UpdateBallPosition(int ballId, float screenWidth, float screenHeight);
    void UpdateAllBalls();
    
    void UpdateScoresAndVelocity();

    void LoadAudioFile();

    void LeftSideDisplayScore(long score);
    void RightSideDisplayScore(long score);

    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_digitBitmaps[TOTAL_DIGITS];
    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_digitBlueBitmaps[TOTAL_DIGITS_BLUE];
    D2D1_RECT_F m_playerScorePositions[MAX_PLAYERS][SCORE_DISPLAY_DIGITS];
    D2D1_RECT_F m_playerLives;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_backgroundBrush;
    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_tableBitmap;
    D2D1_RECT_F m_tablePosition;
 
    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_paddleBitmap;

    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_ballBitmaps[MAX_BALL_BITMAPS][BALL_TRAIL];

    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_countDownBitmaps[WAIT_TIME];
    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_gameOverBitmap;
    D2D1_RECT_F m_countdownRenderPosition;

    float m_paddlePositions[MAX_PLAYERS];
    float m_paddleMovement[MAX_PLAYERS];
    D2D1_RECT_F m_paddleRenderPositions[MAX_PLAYERS];

    long m_goodBallsMissed[MAX_PLAYERS];
    long m_badBallsMissed[MAX_PLAYERS];
    long m_goodBallsHit[MAX_PLAYERS];
    long m_badBallsHit[MAX_PLAYERS];
    long m_gamePoints[MAX_PLAYERS];
    long m_totalGoodBallsMissed;

    int m_ballType[MAX_BALLS];
    int m_maxBallsInGame;
    Windows::Foundation::Point m_ballVelocity[MAX_BALLS];
    D2D1_ELLIPSE m_ballRenderPositions[MAX_BALLS];
    D2D1_RECT_F m_ballBitmapRenderPosition[MAX_BALLS][BALL_TRAIL];
    float m_ballSpeed;

    int m_gamePlayScreen;
    int m_gamePlayDifficulty;
    int m_gameDuration;

    int m_gameEndDuration;
    int m_gameScoreDuration;

	bool m_renderNeeded;

    BasicLoader^ m_basicLoader;
    AudioData^ m_audioCountdown;
    AudioData^ m_audioNewBall;
    AudioData^ m_audioBounce;
    AudioData^ m_audioGoodHit;
    AudioData^ m_audioBadHit;
    AudioData^ m_musicGameStart;
    AudioData^ m_musicGameOver;
};
