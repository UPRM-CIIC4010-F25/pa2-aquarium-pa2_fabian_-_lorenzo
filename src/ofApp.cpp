#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    ofSetBackgroundColor(ofColor::blue);
    backgroundImage.load("background.png");
    backgroundImage.resize(ofGetWindowWidth(), ofGetWindowHeight());

    
    std::shared_ptr<Aquarium> myAquarium;
    std::shared_ptr<PlayerCreature> player;

    // make the game scene manager 
    gameManager = std::make_unique<GameSceneManager>();


    // first we make the intro scene 
    gameManager->AddScene(std::make_shared<GameIntroScene>(
        GameSceneKindToString(GameSceneKind::GAME_INTRO),
        std::make_shared<GameSprite>("title.png", ofGetWindowWidth(), ofGetWindowHeight())
    ));

    //AquariumSpriteManager
    spriteManager = std::make_shared<AquariumSpriteManager>();

    // Lets setup the aquarium
    myAquarium = std::make_shared<Aquarium>(ofGetWindowWidth(), ofGetWindowHeight(), spriteManager);
    player = std::make_shared<PlayerCreature>(ofGetWindowWidth()/2 - 50, ofGetWindowHeight()/2 - 50, DEFAULT_SPEED, this->spriteManager->GetSprite(AquariumCreatureType::NPCreature));
    player->setDirection(0, 0); // Initially stationary
    player->setBounds(ofGetWindowWidth() - 20, ofGetWindowHeight() - 20);

    //  Set powerUpScore values for all levels (powerup target score)
    myAquarium->addAquariumLevel(std::make_shared<Level_0>(0, 10, 5));
    myAquarium->addAquariumLevel(std::make_shared<Level_1>(1, 15, 7));
    myAquarium->addAquariumLevel(std::make_shared<Level_2>(2, 20, 10));
    myAquarium->addAquariumLevel(std::make_shared<Level_3>(3, 35, 17));   //Added implementation of level 3
    myAquarium->addAquariumLevel(std::make_shared<Level_4>(4, 50, 25));   //Added implementation of level 4
    myAquarium->Repopulate(); // initial population


    // now that we are mostly set, lets pass the player and the aquarium downstream
    gameManager->AddScene(std::make_shared<AquariumGameScene>(
        std::move(player), std::move(myAquarium), GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)
    )); // player and aquarium are owned by the scene moving forward

    // Music setup
    if(gameMusic.load("rainyday.mp3") && !musicChanged) {
        gameMusic.setLoop(true);
        gameMusic.play();
        gameMusic.setVolume(0.75f);
        musicChanged = false;
    } 
    else {
        ofLogError() << "Failed to load the awesome game gameMusic!";  // Added some an error message just in case
    }

    // Load font for game over message
    gameOverTitle.load("Verdana.ttf", 12, true, true);
    gameOverTitle.setLineHeight(34.0f);
    gameOverTitle.setLetterSpacing(1.035);


    gameManager->AddScene(std::make_shared<GameOverScene>(
        GameSceneKindToString(GameSceneKind::GAME_OVER),
        std::make_shared<GameSprite>("game-over.png", ofGetWindowWidth(), ofGetWindowHeight())
    ));

    ofSetLogLevel(OF_LOG_NOTICE); // Set default log level
}

//--------------------------------------------------------------
void ofApp::update(){
    if(pausePressed && gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)) return;

    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::GAME_OVER)){
        if(gameMusic.isPlaying()) {
            gameMusic.stop();
        }
        return; // Stop updating if game is over or exiting. The music also stops once game is over.
    }

    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)){
        auto gameScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetActiveScene());

        //Calculated time passed per each frame and added it to the timer
        //Since the game run at 60FPS, one we have 60 frames we will have a second
         musicTimer +=  1.0f/60.0f;

        //Music will change once we have hit two minutes of gameplay
        if(musicTimer >= 60.0f && !musicChanged) {
            if(gameMusic.isPlaying()) gameMusic.stop(); //stops original music to make way for new music
            if(gameMusic.load("Horroriffic.mp3")) {
                gameMusic.setLoop(true);
                gameMusic.play();
                gameMusic.setVolume(0.75f);
                musicChanged = true; //Flag needed so if statement is skipped on future updates
            }
            else {
                ofLogError() << "Failed to load the new awesome game gameMusic!"; //added debug message just in case
            }
        }

        if(gameScene->GetLastEvent() != nullptr && gameScene->GetLastEvent()->isGameOver()){
            gameManager->Transition(GameSceneKindToString(GameSceneKind::GAME_OVER));
            return;
        }
    } 

    gameManager->UpdateActiveScene();
}

//--------------------------------------------------------------
void ofApp::draw(){
    backgroundImage.draw(0, 0);
    gameManager->DrawActiveScene();

    //If flag is true the instructions text will appear if in game mode
    //Once in pause state, literally everything is paused
    if(helpedPressed && gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME) && !pausePressed) {
        ofDrawBitmapString("Use the arrow keys to move your fish around!", 5, 20);  //Added instructions in overlay to improve user experience
        ofDrawBitmapString("PowerUps might appear at some points...", 5, 30);
    
    //Needed so that text would only appear when the instructions text is not present and within the actual game, not intro
    } else if(!helpedPressed && gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME) && !pausePressed) {
        ofDrawBitmapString("Press H to obtain help!!", 5, 20);
    }
    //Since in pause state everything stops, no help is available to make things, more interesting....
    else if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME) && pausePressed) {
        ofDrawBitmapString("Help not available. No advantages here...", 5, 20);
    }
    //If flag is true and within game mode, the pause text will appear
    if(pausePressed && gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)) {
        ofDrawBitmapString("Press P to unpause game!", 5, 50);
        ofDrawBitmapString("You can now breathe...", 5, 60);
    }
    //If flag is false and within game mode, the pause text will change and extra text will be gone... (maybe forever)
    else if(!pausePressed && gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)) {
        ofDrawBitmapString("Press P to pause game!", 5, 50);
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (lastEvent.isGameExit()) { 
        ofLogNotice() << "Game has ended. Press ESC to exit." << std::endl;
        return; // Ignore other keys after game over
    }
    //Added pausePressed condition if not player could move under pause conditions and no cheating!!!
    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME) && !pausePressed){
        auto gameScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetActiveScene());
        switch(key){
            case OF_KEY_UP:
                gameScene->GetPlayer()->setDirection(gameScene->GetPlayer()->isXDirectionActive()?gameScene->GetPlayer()->getDx():0, -1);
                break;
                case OF_KEY_DOWN:
                gameScene->GetPlayer()->setDirection(gameScene->GetPlayer()->isXDirectionActive()?gameScene->GetPlayer()->getDx():0, 1);
                break;
            case OF_KEY_LEFT:
                gameScene->GetPlayer()->setDirection(-1, gameScene->GetPlayer()->isYDirectionActive()?gameScene->GetPlayer()->getDy():0);
                gameScene->GetPlayer()->setFlipped(true);
                break;
                case OF_KEY_RIGHT:
                gameScene->GetPlayer()->setDirection(1, gameScene->GetPlayer()->isYDirectionActive()?gameScene->GetPlayer()->getDy():0);
                gameScene->GetPlayer()->setFlipped(false);
                break;
            default:
                break;
        }

        //Player must keep key pressed to see instructions text
        if(key == 'h' || key == 'H') {
            helpedPressed = true;
        }

        gameScene->GetPlayer()->move();
        return;

    }

    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::GAME_INTRO)){
        switch (key)
        {
        case OF_KEY_SPACE:
            gameManager->Transition(GameSceneKindToString(GameSceneKind::AQUARIUM_GAME));
            break;
        
        default:
            break;
        }
    }


    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)){
        auto gameScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetActiveScene());
    if( key == OF_KEY_UP || key == OF_KEY_DOWN){
        gameScene->GetPlayer()->setDirection(gameScene->GetPlayer()->isXDirectionActive()?gameScene->GetPlayer()->getDx():0, 0);
        gameScene->GetPlayer()->move();
        return;
    }
    
    if(key == OF_KEY_LEFT || key == OF_KEY_RIGHT){
        gameScene->GetPlayer()->setDirection(0, gameScene->GetPlayer()->isYDirectionActive()?gameScene->GetPlayer()->getDy():0);
        gameScene->GetPlayer()->move();
        return;
    }
    //Once player releases key, text will dissapear to make the game more visable and not have a lot of text
    if(key == 'H' || key == 'h') {
        helpedPressed = false;
    }

    //If player presses key once the pause state is activated
    if(key == 'P' || key == 'p') {
    pausePressed = !pausePressed;
    }

    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    backgroundImage.resize(w, h);
    auto aquariumScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetScene(GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)));
    aquariumScene->GetAquarium()->setBounds(w,h);
    aquariumScene->GetPlayer()->setBounds(w - 20, h - 20);

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
