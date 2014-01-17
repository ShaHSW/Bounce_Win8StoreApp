#include "pch.h"
#include "GamePlayRenderer.h"

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;

static const ColorF s_BallColors[] = 
{
    ColorF::AntiqueWhite,
	ColorF::CornflowerBlue,
	ColorF::Orchid,
	ColorF::DeepPink,
	ColorF::Red,
	ColorF::DarkOrange,
	ColorF::Gold,
	ColorF::GreenYellow,
	ColorF::Aqua
};
#define MAX_BALL_COLORS 9

template <class T> 
void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

GamePlayRenderer::GamePlayRenderer() :
	m_renderNeeded(true),
    m_gamePlayScreen(GamePlayScreen::DEFAULT_SCREEN),
    m_gamePlayDifficulty(GamePlayDifficulty::GAME_EASY),
    m_maxBallsInGame(1)
{
    FILETIME time;
    GetSystemTimeAsFileTime(&time);

    srand((int)time.dwLowDateTime);

    for (int index = 0; index < MAX_PLAYERS ; ++index)
    {
        m_paddlePositions[index] = 0.0f;
        m_goodBallsMissed[index] = 0;
        m_goodBallsHit[index] = 0;
        m_badBallsMissed[index] = 0;
        m_badBallsHit[index] = 0;
        m_gamePoints[index] = 0;
        m_paddleMovement[index] = 0.0f;
    }
    
    m_ballType[0] = BallType::BALL_GOOD;
    m_ballType[1] = BallType::BALL_BAD;
    m_ballType[2] = BallType::BALL_NEUTRAL;
}

void GamePlayRenderer::CreateDeviceIndependentResources()
{
	DirectXBase::CreateDeviceIndependentResources();
}

void GamePlayRenderer::CreateDeviceResources()
{
	DirectXBase::CreateDeviceResources();

    m_basicLoader = ref new BasicLoader(m_d2dDevice.Get(), m_d2dContext.Get(), m_wicFactory.Get());

	DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
		    ColorF(ColorF::Black),
		    &m_backgroundBrush
            )
        );

    Platform::Array<Platform::String^>^ ballBitmapPaths = ref new Platform::Array<Platform::String^>{ BALL_BITMAP_1, BALL_BITMAP_2 };

    for (int index = 0; index < (int)ballBitmapPaths->Length; ++index)
    {
        for (int index2 = 0; index2 < BALL_TRAIL; ++index2)
        {
            m_basicLoader->LoadBitmap(
                ballBitmapPaths->get(index),
                &m_ballBitmaps[index][index2],
                (UINT)(BALL_RADIUS * 2), 
                (UINT)(BALL_RADIUS * 2)
                );
        }
    }

    Platform::Array<Platform::String^>^ countdownBitmapPaths = 
        ref new Platform::Array<Platform::String^>{ COUNT_DOWN_3, COUNT_DOWN_2, COUNT_DOWN_1, COUNT_DOWN_GO };

    for (int index = 0; index < (int)countdownBitmapPaths->Length; ++index)
    {
        m_basicLoader->LoadBitmap(
            countdownBitmapPaths->get(index),
            &m_countDownBitmaps[index]
            );
    }

    m_basicLoader->LoadBitmap(
        GAMEOVER_BITMAP,
        &m_gameOverBitmap
        );

    Platform::Array<Platform::String^>^ digitBitmapPaths = 
        ref new Platform::Array<Platform::String^>{
        DIGIT_BITMAPS_0,
        DIGIT_BITMAPS_1,
        DIGIT_BITMAPS_2,
        DIGIT_BITMAPS_3,
        DIGIT_BITMAPS_4,
        DIGIT_BITMAPS_5,
        DIGIT_BITMAPS_6,
        DIGIT_BITMAPS_7,
        DIGIT_BITMAPS_8,
        DIGIT_BITMAPS_9 
        };

    for (int index = 0; index < TOTAL_DIGITS; ++index)
    {
        m_basicLoader->LoadBitmap(
            digitBitmapPaths->get(index),
            &m_digitBitmaps[index]
            );
    }

    Platform::Array<Platform::String^>^ digitBlueBitmapPaths = 
        ref new Platform::Array<Platform::String^>{
        DIGIT_BITMAPS_BLUE_0,
        DIGIT_BITMAPS_BLUE_1,
        DIGIT_BITMAPS_BLUE_2,
        DIGIT_BITMAPS_BLUE_3
        };

    for (int index = 0; index < TOTAL_DIGITS_BLUE; ++index)
    {
        m_basicLoader->LoadBitmap(
            digitBlueBitmapPaths->get(index),
            &m_digitBlueBitmaps[index]
            );
    }
    LoadAudioFile();
}

void GamePlayRenderer::CreateWindowSizeDependentResources()
{
	DirectXBase::CreateWindowSizeDependentResources();

	// Add code to create window size dependent objects here.
    D2D1_SIZE_F screen = m_d2dContext->GetSize();

    m_tablePosition.top = 0.0f;
    m_tablePosition.left = 0.0f;
    m_tablePosition.bottom = screen.height;
    m_tablePosition.right = screen.width;

    D2D1_SIZE_F imgSize = m_countDownBitmaps[0]->GetSize();
    m_countdownRenderPosition.top = (screen.height - imgSize.height) / 2.0f;
    m_countdownRenderPosition.bottom = (screen.height + imgSize.height) / 2.0f;
    m_countdownRenderPosition.left = (screen.width - imgSize.width) / 2.0f;
    m_countdownRenderPosition.right = (screen.width + imgSize.width) / 2.0f;

    m_basicLoader->LoadBitmap(
        TABLE_BITMAP,
        &m_tableBitmap,
        (UINT)(m_tablePosition.right),
        (UINT)(m_tablePosition.bottom)
        );

    float paddleLength = max(screen.height * PADDLE_LENGTH_RELATIVE, PADDLE_LENGTH); 
    
    m_basicLoader->LoadBitmap(
        PADDLE_BITMAP,
        &m_paddleBitmap,
        (UINT)(PADDLE_WIDTH),
        (UINT)(paddleLength)
        );

    
    D2D1_SIZE_F digitBitmapSize = m_digitBitmaps[0]->GetSize();

    for (int index = 0; index < SCORE_DISPLAY_DIGITS; ++index)
    {
        m_playerScorePositions[0][index].top = DISPLAY_MARGIN;
        m_playerScorePositions[0][index].left = CONTROL_SPACE_WIDTH + (index * (digitBitmapSize.width + DISPLAY_MARGIN));
        m_playerScorePositions[0][index].bottom = DISPLAY_MARGIN + digitBitmapSize.height;
        m_playerScorePositions[0][index].right = CONTROL_SPACE_WIDTH + (index * (digitBitmapSize.width + DISPLAY_MARGIN)) + digitBitmapSize.width;
    }

    for (int index = SCORE_DISPLAY_DIGITS - 1; index >= 0; --index)
    {
        m_playerScorePositions[1][SCORE_DISPLAY_DIGITS - index].top = DISPLAY_MARGIN;
        m_playerScorePositions[1][SCORE_DISPLAY_DIGITS - index].left = screen.width - (CONTROL_SPACE_WIDTH + (index * (digitBitmapSize.width + DISPLAY_MARGIN)) + digitBitmapSize.width);
        m_playerScorePositions[1][SCORE_DISPLAY_DIGITS - index].bottom = DISPLAY_MARGIN + digitBitmapSize.height;
        m_playerScorePositions[1][SCORE_DISPLAY_DIGITS - index].right = screen.width - (CONTROL_SPACE_WIDTH + (index * (digitBitmapSize.width + DISPLAY_MARGIN)));
    }

    
    D2D1_SIZE_F digitBlueBitmapSize = m_digitBlueBitmaps[0]->GetSize();

    m_playerLives.top = DISPLAY_MARGIN;
    m_playerLives.left = (screen.width - digitBlueBitmapSize.width)/2.0f;
    m_playerLives.bottom = DISPLAY_MARGIN + digitBlueBitmapSize.height;
    m_playerLives.right = (screen.width + digitBlueBitmapSize.width)/2.0f;

    m_paddleRenderPositions[GamePlayers::PLAYER_1].left   = CONTROL_SPACE_WIDTH;
    m_paddleRenderPositions[GamePlayers::PLAYER_1].right  = CONTROL_SPACE_WIDTH + PADDLE_WIDTH;
    m_paddleRenderPositions[GamePlayers::PLAYER_2].left   = screen.width - CONTROL_SPACE_WIDTH - PADDLE_WIDTH;
    m_paddleRenderPositions[GamePlayers::PLAYER_2].right  = screen.width - CONTROL_SPACE_WIDTH;

    for (int index = 0; index < MAX_BALLS ; ++index)
    {
        m_ballRenderPositions[index].point.x = 
            max(
                CONTROL_SPACE_WIDTH + PADDLE_WIDTH + BALL_RADIUS,
                min(
                    screen.width - CONTROL_SPACE_WIDTH - PADDLE_WIDTH - BALL_RADIUS,
                    m_ballRenderPositions[index].point.x
                    )
                );
    }
}

void GamePlayRenderer::LoadAudioFile()
{
    m_audioCountdown = ref new AudioData();
    m_audioCountdown->LoadAudio(COUNT_DOWN_SOUND, m_soundEffectEngine.Get());

    m_audioBounce = ref new AudioData();
    m_audioBounce->LoadAudio(BOUNCE_WALL_SOUND, m_soundEffectEngine.Get());    
    m_audioNewBall = ref new AudioData();
    m_audioNewBall->LoadAudio(NEW_BALL_SOUND, m_soundEffectEngine.Get());       
    m_audioGoodHit = ref new AudioData();
    m_audioGoodHit->LoadAudio(GOOD_BALL_HIT_SOUND, m_soundEffectEngine.Get());  
    m_audioBadHit = ref new AudioData();
    m_audioBadHit->LoadAudio(BAD_BALL_HIT_SOUND, m_soundEffectEngine.Get());   
    
    m_musicGameStart = ref new AudioData();
    m_musicGameStart->LoadAudio(GAMESTART_MUSIC, m_musicEngine.Get());
    m_musicGameOver = ref new AudioData();
    m_musicGameOver->LoadAudio(GAMEOVER_MUSIC, m_musicEngine.Get());
}

void GamePlayRenderer::SuspendAudio()
{
    if (m_audioAvailable)
    {
        m_musicEngine->StopEngine();
        m_soundEffectEngine->StopEngine();
    }
}

void GamePlayRenderer::ResumeAudio()
{
    if (m_audioAvailable)
    {
        DX::ThrowIfFailed(m_musicEngine->StartEngine());
        DX::ThrowIfFailed(m_soundEffectEngine->StartEngine());
    }
}

void GamePlayRenderer::InitializeGame()
{
    m_totalGoodBallsMissed = 0;

    for (int index = 0; index < MAX_PLAYERS; ++index)
    {
        m_paddlePositions[index] = 0.0f;
        UpdatePaddlePosition(index, 0.5f);
        m_goodBallsMissed[index] = 0;
        m_badBallsMissed[index] = 0;
        m_goodBallsHit[index] = 0;
        m_badBallsHit[index] = 0;
        m_gamePoints[index] = 0;
    }

    m_ballSpeed = DEFAULT_BALL_SPEED;
    switch (m_gamePlayDifficulty)
    {
    case GamePlayDifficulty::GAME_VERY_EASY:
        m_maxBallsInGame = 1;
        m_ballSpeed = DEFAULT_BALL_SPEED;
        break;
    case GamePlayDifficulty::GAME_EASY:
        m_maxBallsInGame = 1;
        m_ballSpeed = DEFAULT_BALL_SPEED + (BALL_SPEED_DELTA * 2.0f);
        break;
    case GamePlayDifficulty::GAME_NORMAL:
        m_maxBallsInGame = 2;
        m_ballSpeed = DEFAULT_BALL_SPEED + (BALL_SPEED_DELTA * 4.0f);
        break;
    case GamePlayDifficulty::GAME_HARD:
        m_maxBallsInGame = 2;
        m_ballSpeed = DEFAULT_BALL_SPEED + (BALL_SPEED_DELTA * 5.0f);
        break;
    case GamePlayDifficulty::GAME_NO_WAY:
        m_maxBallsInGame = 3;
        m_ballSpeed = DEFAULT_BALL_SPEED + (BALL_SPEED_DELTA * 5.0f);
        break;
    }

    D2D1_SIZE_F screenSize = m_d2dContext->GetSize();
    float centerX = screenSize.width / 2.0f;
    float centerY = screenSize.height / 2.0f;
    float moveSpaceHorizontal = (screenSize.width - (2.0f * (CONTROL_SPACE_WIDTH + PADDLE_WIDTH + 2.0f * (BALL_RADIUS + BALL_INIT_BUFFER)))) / 2.0f;
    float moveSpaceVertical = (screenSize.height - (4.0f * BALL_RADIUS) - (2.0f * TABLE_BORDER_HEIGHT)) / 2.0f;

    float randX = 0.0f;
    float randY = 0.0f;

    for (int index = 0; index < MAX_BALLS ; ++index)
    {
        randX = ((float)((rand() % 2) ? -rand() : rand()))/RAND_MAX;
        randY = ((float)((rand() % 2) ? -rand() : rand()))/RAND_MAX;

        m_ballRenderPositions[index].point.x = centerX + (randX * moveSpaceHorizontal) + (((randX < 0.0) ? -1.0f : 1.0f) * BALL_INIT_BUFFER);
        m_ballRenderPositions[index].point.y = centerY + (randY * moveSpaceVertical);

        m_ballRenderPositions[index].radiusX = BALL_RADIUS;
        m_ballRenderPositions[index].radiusY = BALL_RADIUS;

        m_ballVelocity[index].X = m_ballSpeed * ((rand() % 2) ? -1.0f : 1.0f);
        m_ballVelocity[index].Y = m_ballSpeed * ((rand() % 2) ? -1.0f : 1.0f);
    }

    m_ballType[0] = BallType::BALL_GOOD;
    m_ballType[1] = BallType::BALL_BAD;
    m_ballType[2] = BallType::BALL_NEUTRAL;
}


void GamePlayRenderer::Update(float timeTotal, float timeDelta)
{
	(void) timeTotal; // Unused parameter.
	(void) timeDelta; // Unused parameter.

	// Add code to update time dependent objects here.
}

void GamePlayRenderer::Render()
{
	m_d2dContext->BeginDraw();

    m_d2dContext->Clear(m_backgroundBrush->GetColor());

    switch (m_gamePlayScreen)
    {
    case GamePlayScreen::DEFAULT_SCREEN:
        RenderDefaultScreen();
        break;
    case GamePlayScreen::COUNT_DOWN_SCREEN:
        RenderCountDownScreen();
        break;
    case GamePlayScreen::GAME_PLAY_SCREEN:
        RenderGamePlayAgainstScreen();
        break;
    case GamePlayScreen::GAME_OVER_SCREEN:
        RenderGameOverScreen();
        break;
    case GamePlayScreen::GAME_SCORE_SCREEN:
        RenderGameScoreScreen();
        break;
    }

	// Ignore D2DERR_RECREATE_TARGET. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = m_d2dContext->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	m_renderNeeded = false;

    UpdateAllPaddles();
    UpdateAllBalls();
    UpdateScoresAndVelocity();
}

void GamePlayRenderer::UpdateAllPaddles()
{
    switch (m_gamePlayScreen)
    {
    case GamePlayScreen::GAME_PLAY_SCREEN:
        for (int index = 0; index < MAX_PLAYERS; ++index)
        {
            if (m_paddleMovement[index])
            {
                UpdatePaddlePosition(index, m_paddleMovement[index]);
                m_renderNeeded = true;
            }
        }
        break;
    default:
        break;
    }
}

void GamePlayRenderer::UpdateAllBalls()
{
    D2D1_SIZE_F screen = m_d2dContext->GetSize();

    switch (m_gamePlayScreen)
    {
    case GamePlayScreen::GAME_PLAY_SCREEN:
        for (int index = 0; index < m_maxBallsInGame; ++index)
        {
            UpdateBallPosition(index, screen.width, screen.height);
        }
        break;
    default:
        break;
    }
}

void GamePlayRenderer::UpdateBallPosition(int ballId, float screenWidth, float screenHeight)
{
    float leftLimit = CONTROL_SPACE_WIDTH + PADDLE_WIDTH + BALL_RADIUS;
    float rightLimit = screenWidth - CONTROL_SPACE_WIDTH - PADDLE_WIDTH - BALL_RADIUS;
    float topLimit = BALL_RADIUS + TABLE_BORDER_HEIGHT;
    float bottomLimit = screenHeight - BALL_RADIUS - TABLE_BORDER_HEIGHT;

    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    float moveSpaceHorizontal = (screenWidth - (2.0f * (CONTROL_SPACE_WIDTH + PADDLE_WIDTH + 2.0f * (BALL_RADIUS + BALL_INIT_BUFFER)))) / 2.0f;
    float moveSpaceVertical = (screenHeight - (4.0f * BALL_RADIUS) - (2.0f * TABLE_BORDER_HEIGHT)) / 2.0f;

    float randX = 0.0f;
    float randY = 0.0f;

    float oldX = m_ballRenderPositions[ballId].point.x;
    float oldY = m_ballRenderPositions[ballId].point.y;

    float newX = oldX + m_ballVelocity[ballId].X;
    float newY = oldY + m_ballVelocity[ballId].Y;

    if ((newY < topLimit) || (newY > bottomLimit)) 
    {
        m_ballVelocity[ballId].Y = -m_ballVelocity[ballId].Y;
        PlayBounce();
    }

    newY = oldY + m_ballVelocity[ballId].Y; 

    // check for paddle for player1
    if (newX < leftLimit)
    {
        // the player hit the ball
        if ((m_paddleRenderPositions[GamePlayers::PLAYER_1].top < newY) &&
            (m_paddleRenderPositions[GamePlayers::PLAYER_1].bottom > newY))
        {
            switch (m_ballType[ballId])
            {
            case BallType::BALL_GOOD: 
                m_goodBallsHit[GamePlayers::PLAYER_1]++;
                PlayGoodHit();
                break;
            case BallType::BALL_BAD:
                m_badBallsHit[GamePlayers::PLAYER_1]++;
                PlayBadHit();
                break;
            }

            m_ballVelocity[ballId].X = -m_ballVelocity[ballId].X;
            newX = oldX + m_ballVelocity[ballId].X;
            m_ballRenderPositions[ballId].point.x = newX;
            m_ballRenderPositions[ballId].point.y = newY;
        }
        else
        {
            // the player missed the ball
            switch (m_ballType[ballId])
            {
            case BallType::BALL_GOOD: 
                m_goodBallsMissed[GamePlayers::PLAYER_1]++;
                m_ballVelocity[ballId].X = m_ballSpeed * ((rand() % 2) ? -1.0f : 1.0f);
                m_ballVelocity[ballId].Y = m_ballSpeed * ((rand() % 2) ? -1.0f : 1.0f);
                randX = ((float)((rand() % 2) ? -rand() : rand()))/RAND_MAX;
                randY = ((float)((rand() % 2) ? -rand() : rand()))/RAND_MAX;
                m_ballRenderPositions[ballId].point.x = centerX + (randX * moveSpaceHorizontal) + (((randX < 0.0) ? -1.0f : 1.0f) * BALL_INIT_BUFFER);
                m_ballRenderPositions[ballId].point.y = centerY + (randY * moveSpaceVertical);
                PlayNewBall();
                break;
            case BallType::BALL_BAD: // the bad ball is bounced back even if you miss it
                m_badBallsMissed[GamePlayers::PLAYER_1]++;
                m_ballVelocity[ballId].X = -m_ballVelocity[ballId].X;
                newX = oldX + m_ballVelocity[ballId].X;
                m_ballRenderPositions[ballId].point.x = newX;
                m_ballRenderPositions[ballId].point.y = newY;
                PlayBounce();
                break;
            }
        }
    }
    // check for paddle for player2
    else if (newX > rightLimit)
    {
        // the player hit the ball
        if ((m_paddleRenderPositions[GamePlayers::PLAYER_2].top < newY) &&
            (m_paddleRenderPositions[GamePlayers::PLAYER_2].bottom > newY))
        {
            switch (m_ballType[ballId])
            {
            case BallType::BALL_GOOD: 
                m_goodBallsHit[GamePlayers::PLAYER_2]++;
                PlayGoodHit();
                break;
            case BallType::BALL_BAD:
                m_badBallsHit[GamePlayers::PLAYER_2]++;
                PlayBadHit();
                break;
            }

            m_ballVelocity[ballId].X = -m_ballVelocity[ballId].X;
            newX = oldX + m_ballVelocity[ballId].X;
            m_ballRenderPositions[ballId].point.x = newX;
            m_ballRenderPositions[ballId].point.y = newY;
        }
        else
        {
            // the player missed the ball
            switch (m_ballType[ballId])
            {
            case BallType::BALL_GOOD: 
                m_goodBallsMissed[GamePlayers::PLAYER_2]++;
                m_ballVelocity[ballId].X = m_ballSpeed * ((rand() % 2) ? -1.0f : 1.0f);
                m_ballVelocity[ballId].Y = m_ballSpeed * ((rand() % 2) ? -1.0f : 1.0f);
                randX = ((float)((rand() % 2) ? -rand() : rand()))/RAND_MAX;
                randY = ((float)((rand() % 2) ? -rand() : rand()))/RAND_MAX;
                m_ballRenderPositions[ballId].point.x = centerX + (randX * moveSpaceHorizontal) + (((randX < 0.0) ? -1.0f : 1.0f) * BALL_INIT_BUFFER);
                m_ballRenderPositions[ballId].point.y = centerY + (randY * moveSpaceVertical);
                PlayNewBall();
                break;
            case BallType::BALL_BAD: // the bad ball is bounced back even if you miss it
                m_badBallsMissed[GamePlayers::PLAYER_2]++;
                m_ballVelocity[ballId].X = -m_ballVelocity[ballId].X;
                newX = oldX + m_ballVelocity[ballId].X;
                m_ballRenderPositions[ballId].point.x = newX;
                m_ballRenderPositions[ballId].point.y = newY;
                PlayBounce();
                break;
            }
        }
    }
    // Just move the ball
    else 
    {
        m_ballRenderPositions[ballId].point.x = newX;
        m_ballRenderPositions[ballId].point.y = newY;
    }

    for(int index = BALL_TRAIL - 1; index > 0; --index)
    {
        m_ballBitmapRenderPosition[ballId][index].left   = m_ballBitmapRenderPosition[ballId][index - 1].left;   
        m_ballBitmapRenderPosition[ballId][index].top    = m_ballBitmapRenderPosition[ballId][index - 1].top;    
        m_ballBitmapRenderPosition[ballId][index].right  = m_ballBitmapRenderPosition[ballId][index - 1].right;  
        m_ballBitmapRenderPosition[ballId][index].bottom = m_ballBitmapRenderPosition[ballId][index - 1].bottom;
    }

    m_ballBitmapRenderPosition[ballId][0].left   = newX - BALL_RADIUS;
    m_ballBitmapRenderPosition[ballId][0].top    = newY - BALL_RADIUS;
    m_ballBitmapRenderPosition[ballId][0].right  = newX + BALL_RADIUS;
    m_ballBitmapRenderPosition[ballId][0].bottom = newY + BALL_RADIUS;

    m_renderNeeded = true;
}

void GamePlayRenderer::UpdateScoresAndVelocity()
{
    if (m_gamePlayScreen == GamePlayScreen::GAME_PLAY_SCREEN)
    {
        int totalGoodBallsMissed = 0;
        for (int index = 0; index < MAX_PLAYERS; ++index)
        {
            totalGoodBallsMissed += m_goodBallsMissed[index];

            switch (m_gamePlayDifficulty)
            {
            case GamePlayDifficulty::GAME_VERY_EASY:
                m_gamePoints[index] = m_goodBallsHit[index];
                break;

            case GamePlayDifficulty::GAME_EASY:
                m_gamePoints[index] = m_goodBallsHit[index];
                break;

            case GamePlayDifficulty::GAME_NORMAL:
            case GamePlayDifficulty::GAME_HARD:
                m_gamePoints[index] = 
                    m_goodBallsHit[index] - 
                    m_badBallsHit[index] - 
                    (3 * m_goodBallsMissed[index]) + 
                    m_badBallsMissed[index];
                break;
            }
        }

        if (totalGoodBallsMissed >= MAX_BALLS_MISSED)
        {
            m_gamePlayScreen = GamePlayScreen::GAME_OVER_SCREEN;
            PlayGameOver();
        }

        m_totalGoodBallsMissed = totalGoodBallsMissed;
    }
}

void GamePlayRenderer::UpdatePaddlePosition(int player, float delta)
{
    if (((player >= 0) && (player < MAX_PLAYERS)) &&
        ((delta >= -1.0f) && (delta <= 1.0f)))
    {
        float position = max(0.0f, min(1.0f, m_paddlePositions[player] + delta));
        D2D1_SIZE_F screenSize = m_d2dContext->GetSize();

        m_paddlePositions[player] = position;
        float paddleLength = max(screenSize.height * PADDLE_LENGTH_RELATIVE, PADDLE_LENGTH); 
        float paddleShiftHeight = screenSize.height - paddleLength - (TABLE_BORDER_HEIGHT * 2.0f);
        float paddlePosition = TABLE_BORDER_HEIGHT+ (paddleShiftHeight * position);

        switch (player)
        {
        case GamePlayers::PLAYER_1:
            m_paddleRenderPositions[player].top    = paddlePosition;
            m_paddleRenderPositions[player].left   = CONTROL_SPACE_WIDTH;
            m_paddleRenderPositions[player].bottom = paddleLength + paddlePosition;
            m_paddleRenderPositions[player].right  = CONTROL_SPACE_WIDTH + PADDLE_WIDTH;
            break;
        case GamePlayers::PLAYER_2:
            m_paddleRenderPositions[player].top    = paddlePosition;
            m_paddleRenderPositions[player].left   = screenSize.width - CONTROL_SPACE_WIDTH - PADDLE_WIDTH;
            m_paddleRenderPositions[player].bottom = paddleLength + paddlePosition;
            m_paddleRenderPositions[player].right  = screenSize.width - CONTROL_SPACE_WIDTH;
            break;
        }
    }

    m_renderNeeded = true;
}

void GamePlayRenderer::UpdatePaddleMovementDelta(int player, float delta)
{
    switch(player)
    {
    case GamePlayers::PLAYER_1:
    case GamePlayers::PLAYER_2:
        m_paddleMovement[player] = max(-DEFAULT_PADDLE_MOVEMENT_SPEED, min(DEFAULT_PADDLE_MOVEMENT_SPEED, delta));
        break;
    }
}

void GamePlayRenderer::SaveInternalState(IPropertySet^ state)
{
	/*if (state->HasKey("m_backgroundColorIndex"))
	{
		state->Remove("m_backgroundColorIndex");
	}
	if (state->HasKey("m_textPosition"))
	{
		state->Remove("m_textPosition");
	}
	state->Insert("m_backgroundColorIndex", PropertyValue::CreateInt32(m_backgroundColorIndex));
	state->Insert("m_textPosition", PropertyValue::CreatePoint(m_textPosition));*/
}

void GamePlayRenderer::LoadInternalState(IPropertySet^ state)
{
	/*if (state->HasKey("m_backgroundColorIndex") && state->HasKey("m_textPosition"))
	{
		m_backgroundColorIndex = safe_cast<IPropertyValue^>(state->Lookup("m_backgroundColorIndex"))->GetInt32();
		m_textPosition = safe_cast<IPropertyValue^>(state->Lookup("m_textPosition"))->GetPoint();
	}*/
}

void GamePlayRenderer::RenderDefaultScreen()
{

}

void GamePlayRenderer::RenderCountDownScreen()
{
    m_d2dContext->DrawBitmap(
        m_tableBitmap.Get(),
        m_tablePosition);

    switch (m_gameDuration)
    {
    case 0:
        // Countdown start screen. starting at #3
        m_d2dContext->DrawBitmap(
            m_countDownBitmaps[0].Get(),
            m_countdownRenderPosition);
        break;
    case 1:
        // Countdown next screen. showing #2
        m_d2dContext->DrawBitmap(
            m_countDownBitmaps[1].Get(),
            m_countdownRenderPosition);
        break;
    case 2:
        // Countdown next screen. showing #1
        m_d2dContext->DrawBitmap(
            m_countDownBitmaps[2].Get(),
            m_countdownRenderPosition);
        break;
    case 3:
        // Countdown Last screen. showing GO
        m_d2dContext->DrawBitmap(
            m_countDownBitmaps[3].Get(),
            m_countdownRenderPosition);
        break;
    default:
        m_gamePlayScreen = GamePlayScreen::GAME_PLAY_SCREEN;
        break;
    }
}

void GamePlayRenderer::RenderGamePlayAgainstScreen()
{
    m_d2dContext->DrawBitmap(
        m_tableBitmap.Get(),
        m_tablePosition);

    for (int index = 0; index < MAX_PLAYERS; ++index)
    {
        m_d2dContext->DrawBitmap(
            m_paddleBitmap.Get(),
            m_paddleRenderPositions[index]);
    }

    for (int index = 0; index < m_maxBallsInGame; ++index)
    {
        for (int index2 = BALL_TRAIL - 1; index2 >= 0 ; --index2)
        {
            if ((!m_ballBitmapRenderPosition[index][index2].left) &&
                (!m_ballBitmapRenderPosition[index][index2].top) &&
                (!m_ballBitmapRenderPosition[index][index2].right) &&
                (!m_ballBitmapRenderPosition[index][index2].bottom)) 
                continue;

            m_d2dContext->DrawBitmap(
                m_ballBitmaps[index % MAX_BALL_BITMAPS][index2].Get(), 
                m_ballBitmapRenderPosition[index][index2], 
                index2 ? 0.2f - index2 * 0.04f : 1.0f);
        }
    }

    if (m_totalGoodBallsMissed <= MAX_BALLS_MISSED)
    {
        m_d2dContext->DrawBitmap(
            m_digitBlueBitmaps[MAX_BALLS_MISSED - m_totalGoodBallsMissed].Get(),
            m_playerLives);
    }

    LeftSideDisplayScore(m_gamePoints[0]);
    RightSideDisplayScore(m_gamePoints[1]);

    m_gameEndDuration = m_gameDuration;
}

void GamePlayRenderer::RenderGameOverScreen()
{
    m_d2dContext->DrawBitmap(
        m_tableBitmap.Get(),
        m_tablePosition);

    m_d2dContext->DrawBitmap(
        m_gameOverBitmap.Get(),
        m_countdownRenderPosition);

    if ((m_gameDuration - m_gameEndDuration) > GAME_END_WAIT_TIME)
    {
        m_gamePlayScreen = GamePlayScreen::GAME_SCORE_SCREEN;
        m_gameScoreDuration = m_gameDuration;
    }
}

void GamePlayRenderer::RenderGameScoreScreen()
{
    if ((m_gameDuration - m_gameScoreDuration) > GAME_SCORE_WAIT_TIME)
    {
        m_gamePlayScreen = GamePlayScreen::DEFAULT_SCREEN;
        GameOver(this, nullptr);
    }
}

void GamePlayRenderer::UpdateBallVelocities()
{
    switch (m_gamePlayDifficulty)
    {
    case GamePlayDifficulty::GAME_VERY_EASY:
        break;
    case GamePlayDifficulty::GAME_EASY:
        for (int index = 0; index < MAX_BALLS; ++index)
        {
            m_ballVelocity[index].X = min(MAX_BALL_SPEED, max(MIN_BALL_SPEED, m_ballVelocity[index].X + BALL_SPEED_DELTA)); 
            m_ballVelocity[index].Y = min(MAX_BALL_SPEED, max(MIN_BALL_SPEED, m_ballVelocity[index].Y + BALL_SPEED_DELTA)); 
        }
        break;
    case GamePlayDifficulty::GAME_NORMAL:
        break;
    case GamePlayDifficulty::GAME_HARD:
        for (int index = 0; index < MAX_BALLS; ++index)
        {
            m_ballVelocity[index].X = min(MAX_BALL_SPEED, max(MIN_BALL_SPEED, m_ballVelocity[index].X + BALL_SPEED_DELTA)); 
            m_ballVelocity[index].Y = min(MAX_BALL_SPEED, max(MIN_BALL_SPEED, m_ballVelocity[index].Y + BALL_SPEED_DELTA)); 
        }
        break;
    default:
        break;
    }

    m_renderNeeded = true;
}

void GamePlayRenderer::LeftSideDisplayScore(long score)
{
    int scoreDigits[SCORE_DISPLAY_DIGITS] = 
    {
        (score / 100000000) % 10,
        (score / 10000000 ) % 10,
        (score / 1000000  ) % 10,
        (score / 100000   ) % 10,
        (score / 10000    ) % 10,
        (score / 1000     ) % 10,
        (score / 100      ) % 10,
        (score / 10       ) % 10,
        (score / 1        ) % 10,
    };
    
    bool skip = true;
    for (int index = 0, dindex = 0; index < SCORE_DISPLAY_DIGITS; ++index)
    {
        if (scoreDigits[index] > 0)
            skip = false;
        
        if (!skip)
        {
            m_d2dContext->DrawBitmap(
                m_digitBitmaps[scoreDigits[index]].Get(),
                m_playerScorePositions[0][dindex++]
                );
        }
    }

    if (skip)
    {
        m_d2dContext->DrawBitmap(
            m_digitBitmaps[0].Get(),
            m_playerScorePositions[0][0]
            );
    }
}

void GamePlayRenderer::RightSideDisplayScore(long score)
{
    int scoreDigits[SCORE_DISPLAY_DIGITS] = 
    {
        (score / 100000000) % 10,
        (score / 10000000 ) % 10,
        (score / 1000000  ) % 10,
        (score / 100000   ) % 10,
        (score / 10000    ) % 10,
        (score / 1000     ) % 10,
        (score / 100      ) % 10,
        (score / 10       ) % 10,
        (score / 1        ) % 10,
    };

    bool skip = true;
    for (int index = 0; index < SCORE_DISPLAY_DIGITS; ++index)
    {
        if (scoreDigits[index] > 0)
            skip = false;
        
        if (!skip)
        {
            m_d2dContext->DrawBitmap(
                m_digitBitmaps[scoreDigits[index]].Get(),
                m_playerScorePositions[1][index]
                );
        }
    }
    if (skip)
    {
        m_d2dContext->DrawBitmap(
            m_digitBitmaps[0].Get(),
            m_playerScorePositions[1][0]
            );
    }
}