# NGame-Engine

	Hello if you are reading this you wanted to try out what my 15 years old ass did, dont expect to much
		First I must tell I did all of the project by reading from
		https://learnopengl.com but I need to stop before I got into the 
		advanced lightings becouse of the main engines functionalitys 
		like component system game object ext

		I tried to implement a cpp code reader first I tried C# but that was soo fckng hard
		soo I give up and changed with c++ code reader but I coudnt do that to lol :p

		soo I just gave up all it only take 2 days or some sht to make this game engine or what I call is NGame Engine
		stands for Not a game engine :D

		anyways I want to show how to use it 

		first you have out of the project and inside
		firstof before going inside of the project I want to say I dont use cmake for this project becouse I'm to lazy
		and I need to say some part of the code I make it local paths so You gonna find and change it D:

		Sorry

		outside of the engine on main.cpp you can use functions like the down bellow

			Engine.bDebugMode = true / false;
			Engine.createGame("RANDOM NAME", WIDTH, HEIGHT);
			Engine.scene->AddGameObject("RANDOM NAME");
			Engine.scene->GetGameObject("RANDOM NAME");
			Engine.scene->GetGameObject("RANDOM NAME")->AddComponentByNameToObject(
				GameObject::RegisterComponent("Rigid Body", []() { return std::make_shared<CRigidBodyComponent>(); });
				GameObject::RegisterComponent("Box Collision", []() { return std::make_shared<CBoxCollisionComponent>(); });
				GameObject::RegisterComponent("Static Mesh", []() { return std::make_shared<CStaticMeshComponent>(); });
				GameObject::RegisterComponent("BDScript", []() { return std::make_shared<CScriptingComponent>(); });

				this is the all registered names for components
			)
			Engine.scene->GetGameObject("RANDOM NAME")->AddComponent<CStaticMeshComponent>();
			Engine.scene->GetGameObject("RANDOM NAME")->AddComponent<CRigidBodyComponent>()->ApplyForce(LIKE THISSS);

			Engine.update();

		I'm not gonna explain all of it...

		But inside of the engine you gonna do all of that inside of the GUI
		jsut click the game objects on top right and add game object add static mesh component to object by
		clicking the object on the viewport or on the scene browser and go down your gonna see a search bar
		write static mesh, static, rigid, box collider, collider any think you want 

		if you wanna create your own component I created a template for you inside of the component file inside of the 
		engine/include/gameengine/components

		anyways this is all I'm just a chill guy who wants to learn openGL

		Thanks