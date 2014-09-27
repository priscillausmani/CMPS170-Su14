
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    // Define some constants
    const float pi = 3.14159f;
    const int gameWidth = 800;
    const int gameHeight = 600;
    const int totalBall = 5;
    sf::Vector2f paddleSize(25, 200);
    float ballRadius = 10.f;

    // Create the window of the application
    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "5 balls 2 players pong");
    window.setVerticalSyncEnabled(true);

    // Load the sounds used in the game
    sf::SoundBuffer ballSoundBuffer;
    if (!ballSoundBuffer.loadFromFile("resources/ball.wav"))
        return EXIT_FAILURE;
    sf::Sound ballSound(ballSoundBuffer);

    // Create the left paddle
    sf::RectangleShape leftPaddle;
    leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    leftPaddle.setOutlineThickness(3);
    leftPaddle.setOutlineColor(sf::Color::Black);
    leftPaddle.setFillColor(sf::Color(100, 100, 200));
    leftPaddle.setOrigin(paddleSize / 2.f);

    // Create the right paddle
    sf::RectangleShape rightPaddle;
    rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    rightPaddle.setOutlineThickness(3);
    rightPaddle.setOutlineColor(sf::Color::Black);
    rightPaddle.setFillColor(sf::Color(200, 100, 100));
    rightPaddle.setOrigin(paddleSize / 2.f);

    // Create the ball
    sf::CircleShape ball[totalBall];
    
    for (int i=0 ; i<totalBall ; ++i )
    {
      ball[i].setRadius(ballRadius - 3);
      ball[i].setOutlineThickness(3);
      ball[i].setOutlineColor(sf::Color::Black);
      ball[i].setFillColor(sf::Color(255-(i*20),255-(i*20),255-(i*20))); //shades of grey
      ball[i].setOrigin(ballRadius / 2, ballRadius / 2);
    }



    // Load the text font
    sf::Font font;
    if (!font.loadFromFile("resources/Arial.ttf"))
        return EXIT_FAILURE;

    // Initialize the pause message
    sf::Text pauseMessage;
    pauseMessage.setFont(font);
    pauseMessage.setCharacterSize(40);
    pauseMessage.setPosition(170.f, 150.f);
    pauseMessage.setColor(sf::Color::White);
    pauseMessage.setString("Welcome to 5-balls pong!\nPress space to start the game\n\nPlayer Left: Q-up S-down\n Player Right:P-up L-down");

    // Define the paddles properties
    sf::Clock AITimer;
    const sf::Time AITime   = sf::seconds(0.1f);
    const float paddleSpeed = 800.f;
    const float ballSpeedConstant = 100.f;
    float ballSpeed[totalBall];
    float ballAngle[totalBall];
    for ( int i=0 ; i<totalBall ; ++i ) //initialize
    {
      ballSpeed[i]=ballSpeedConstant;
      ballAngle[i]=0.f;
    }

    sf::Clock clock;
    bool isPlaying = false;
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) || 
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }

            // Space key pressed: play
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
            {
                if (!isPlaying)
                {
                    // (re)start the game
                    isPlaying = true;
                    clock.restart();

                    // Reset the position of the paddles and ball
                    leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
                    rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);

                    for ( int i=0 ; i<totalBall ; ++i )
                    {
                     ball[i].setPosition(gameWidth / 2, gameHeight / 2);
                    }

                    // Reset the ball angle
                    for ( int i=0 ; i<totalBall ; ++i )
                    {
                      do
                      {
                        // Make sure the ball initial angle is not too much vertical
                        ballAngle[i] = (std::rand() % 360) * 2 * pi / 360;
                      }
                      while (std::abs(std::cos(ballAngle[i])) < 0.7f);
                    }
                }
            }
        }

        if (isPlaying)
        {
            float deltaTime = clock.restart().asSeconds();

            // Move the player1 paddle
            // Up
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) &&
               (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
            {
                leftPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            // Down
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
               (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
            {
                leftPaddle.move(0.f, paddleSpeed * deltaTime);
            }

            // Move the player2 paddle
            // Up
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) &&
               (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
            {
                rightPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            // Down
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) &&
               (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
            {
                rightPaddle.move(0.f, paddleSpeed * deltaTime);
            }

            // Move the ball
            for ( int i=0 ; i<totalBall ; ++i )
            {
              ball[i].move(std::cos(ballAngle[i]) * (ballSpeed[i] * deltaTime), std::sin(ballAngle[i]) * (ballSpeed[i] * deltaTime));
            }

            for ( int i=0 ; i<totalBall ; ++i )
            {

            // Check collisions between the ball and the screen
              if ( ball[i].getPosition().x - ballRadius < 0.f) 
              {
                isPlaying = false;
                pauseMessage.setString("Red won!\nPress space to restart or\nescape to exit");
                break;
              }
              if ( ball[i].getPosition().x + ballRadius > gameWidth )
              {
                isPlaying = false;
                pauseMessage.setString("Blue won!\nPress space to restart or\nescape to exit");
                break;
              }
              if (ball[i].getPosition().y - ballRadius < 0.f)
              {
                ballSound.play();
                ballAngle[i] = -ballAngle[i];
                ball[i].setPosition(ball[i].getPosition().x, ballRadius + 0.1f);
              }

              if (ball[i].getPosition().y + ballRadius > gameHeight)
              {
                ballSound.play();
                ballAngle[i] = -ballAngle[i];
                ball[i].setPosition(ball[i].getPosition().x, gameHeight - ballRadius - 0.1f);
              }


              // Check the collisions between the ball and the paddles
              // Left Paddle
              if (ball[i].getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x / 2 && 
                ball[i].getPosition().x - ballRadius > leftPaddle.getPosition().x &&
                ball[i].getPosition().y + ballRadius >= leftPaddle.getPosition().y - paddleSize.y / 2 &&
                ball[i].getPosition().y - ballRadius <= leftPaddle.getPosition().y + paddleSize.y / 2)
              {
                if (ball[i].getPosition().y > leftPaddle.getPosition().y)
                    ballAngle[i] = pi - ballAngle[i] + (std::rand() % 20) * pi / 180;
                else
                    ballAngle[i] = pi - ballAngle[i] - (std::rand() % 20) * pi / 180;
                //adjust speed according to ball hit location
                //center is the strongest 
                float speedFactor=0.f;
                speedFactor=(std::abs(ball[i].getPosition().y - leftPaddle.getPosition().y)) / paddleSize.y;
                ballSpeed[i]=ballSpeedConstant - (speedFactor * 10);

                ballSound.play();
                ball[i].setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball[i].getPosition().y);
              }

              // Right Paddle
              if (ball[i].getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x / 2 &&
                ball[i].getPosition().x + ballRadius < rightPaddle.getPosition().x &&
                ball[i].getPosition().y + ballRadius >= rightPaddle.getPosition().y - paddleSize.y / 2 &&
                ball[i].getPosition().y - ballRadius <= rightPaddle.getPosition().y + paddleSize.y / 2)
              {
                if (ball[i].getPosition().y > rightPaddle.getPosition().y)
                    ballAngle[i] = pi - ballAngle[i] + (std::rand() % 20) * pi / 180;
                else
                    ballAngle[i] = pi - ballAngle[i] - (std::rand() % 20) * pi / 180;
                float speedFactor=0.f;
                speedFactor=(std::abs(ball[i].getPosition().y - leftPaddle.getPosition().y)) / paddleSize.y;
                ballSpeed[i]=ballSpeedConstant - (speedFactor * 10);

                ballSound.play();
                ball[i].setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball[i].getPosition().y);
              }

              // Check the collisions between the balls
              for (int j=i+1; j<totalBall; ++j) 
              {
                if (
                   (std::abs(ball[i].getPosition().x - ball[j].getPosition().x) < ballRadius )
                   && 
                   (std::abs(ball[i].getPosition().y - ball[j].getPosition().y) < ballRadius )
                   ) 
                   {
                     ballAngle[i] = pi - ballAngle[i] + (std::rand() % 20) * pi / 180;
                     ballAngle[j] = pi - ballAngle[j] + (std::rand() % 20) * pi / 180;
                   }
                }

              } 

          }//for total ball loop


        // Clear the window
        window.clear(sf::Color(50, 200, 50));

        if (isPlaying)
        {
            // Draw the paddles and the ball
            window.draw(leftPaddle);
            window.draw(rightPaddle);

            for ( int i=0 ; i<totalBall ; ++i )
            {
              window.draw(ball[i]);
            }
        }
        else
        {
            // Draw the pause message
            window.draw(pauseMessage);
        }

        // Display things on screen
        window.display();
    }

    return EXIT_SUCCESS;
}
