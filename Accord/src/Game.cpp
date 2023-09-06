#include <Cockroach.h>
#include <Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EntityDef.h"
#include "EditorCursor.h"

#include "Game.h"

PlayerData Game::data;

CameraController* Game::cameraController = nullptr;
Player* Game::player = nullptr;
Entities::Particles* Game::particles = nullptr;

Ref<Framebuffer> Game::framebuffer = nullptr;
Ref<Texture2D> Game::baseSpriteSheet = nullptr;
Ref<Texture2D> Game::tilemapSheet = nullptr;
Ref<Texture2D> Game::background = nullptr;

std::vector<Sprite> Game::entitySprites;
std::vector<Sprite> Game::decorationSprites;

std::vector<Ref<Room>> Game::rooms;

Timer Game::freezeTimer = Timer(0.0f);

bool Game::editMode = true;

Game::Game()
	: Application()
{
	Game::framebuffer = CreateRef<Framebuffer>(320, 180);
	Game::background = CreateRef<Texture2D>("assets/textures/BG_RED.png");
	Game::baseSpriteSheet = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
	Game::tilemapSheet = CreateRef<Texture2D>("assets/textures/Tilemaps.png");

	LoadSprites();

	Application::GetWindow().SetWindowMode(data.fullscreen);
	Audio::ToggleSoundMaster(data.mutedMaster);
	Audio::ToggleSoundSFX(data.mutedSFX);
	Audio::ToggleSoundMusic(data.mutedMusic);

	//for (int i = EntityType::Checkpoint; i < EntityType::END; i++)
	//{
	//	Entity* e = CreateEntity(EntityDefinition(i, false, ZEROi, ONEi));
	//	if (entitySprites.size() < i + 1)
	//		entitySprites.resize(i + 1);
	//	if (e->sprite.texture)
	//		entitySprites[i] = e->sprite;
	//	else
	//		entitySprites[i] = Sprite::CreateFromCoords(baseSpriteSheet, { 4, 2 }, { 8,8 });
	//}
	//SaveSprites();

	Game::player = new Entities::Player(data.playerPosition, {6,0}, {10,12});
	player->type = EntityType::Payga;
	player->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0, 3 }, { 16, 16 });

	Game::cameraController = new CameraController();
	cameraController->type = EntityType::Camera;
	cameraController->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0,0 }, { 1,1 });
	cameraController->positionHighRes = data.playerPosition;

	Game::particles = new Entities::Particles();
	particles->type = EntityType::Particles;

	const std::filesystem::path roomDir{ "assets/rooms" };
	for (auto& a : std::filesystem::directory_iterator(roomDir))
	{
		std::string name = a.path().string().substr(a.path().string().find_last_of("/\\") + 1);
		rooms.push_back(Room::Load(name));
	}

	for (auto& room : rooms)
		if (room->Contains(player->WorldHitbox()))
			Room::current = room;

	static Sound sound = Sound("assets/audio/SheerIceTorrent.wav", true);
	sound.Start();
}

void Game::Update(float dt)
{
	if (!freezeTimer.Finished())
	{
		freezeTimer.Tick(1.0f);
		return;
	}

	if (cameraController->isTransitioning) 
	{
		cameraController->Update(dt);
		return;
	}

	if (!editMode)
	{
		player->Update(dt);
		particles->Update(dt);
	}
	
	Room::current->Update(dt);
	cameraController->Update(dt);

	EditorCursor::Update(dt);

	if (Input::IsDown(CR_KEY_TAB))
	{
		cameraController->SetZoom(90.0f);
		editMode = !editMode;
	}

	if (Input::IsDown(CR_KEY_F))
	{
		Application::GetWindow().SetWindowMode(!Application::GetWindow().fullscreen);
		data.fullscreen = Application::GetWindow().fullscreen;
		data.Save();
	}
	if (Input::IsDown(CR_KEY_M))
	{
		Audio::ToggleSoundMaster(data.mutedMaster = !data.mutedMaster);
		data.Save();
	}

	if (Input::IsDown(CR_KEY_G)) renderGrid = !renderGrid;
	if (Input::IsDown(CR_KEY_H)) renderHitboxes = !renderHitboxes;
	if (Input::IsDown(CR_KEY_R)) renderAllRooms = !renderAllRooms;
}

#pragma warning (disable: 4244) // No need for DrawQuad... warnings

void Game::Render()
{
	if (!editMode)
	{
		framebuffer->Bind();
		glViewport(0, 0, 320, 180);
	}

	Renderer::SetClearColor({ 0.1f, 0.0f, 0.0f, 1 });
	Renderer::Clear();
	glEnable(GL_DEPTH_TEST);

	Renderer::BeginScene(cameraController->camera);

	float zoom = cameraController->zoom;
	float aspect = cameraController->aspectRatio;

	Sprite backgroundSprite = Sprite(background, { 0,0 }, { 1,1 });
	Renderer::DrawQuad(float3(cameraController->positionHighRes, -90.0f)-float3(aspect*zoom,zoom,0), 2.0f * float2(aspect*zoom,zoom), backgroundSprite); // Turn this into a dedicated function: RenderBackground(Sprite background)?
	
	if (renderGrid) RenderGrid();

	bool renderAllVisibleRooms = cameraController->isTransitioning;

	for (int i = 0; i < rooms.size(); i++)
	{
		bool roomVisible = rooms[i]->OverlapsWith(cameraController->Bounds(), 0, 0);
		if (rooms[i] == Room::current || renderAllRooms || (renderAllVisibleRooms && roomVisible))
		{
			rooms[i]->Render(Game::tilemapSheet);

			if (renderRoomBoundaries)
			{
				Rect bounds = rooms[i]->Bounds();
				Renderer::DrawQuadOutline(bounds.min.x, bounds.max.x, bounds.min.y, bounds.max.y, GREEN);
			}
		}
	}

	player->Render();
	particles->Render();

	if (renderHitboxes) RenderHitboxes();

	for (auto& ent : Room::current->entities)
	{
		if (renderHitboxes && ent->type >= EntityType::TurbineLeft && ent->type <= EntityType::TurbineUp)
		{
			Renderer::DrawQuadOutline(ent->As<Turbine>()->turbineRect.min.x, ent->As<Turbine>()->turbineRect.max.x, ent->As<Turbine>()->turbineRect.min.y, ent->As<Turbine>()->turbineRect.max.y, GREEN);
		}
	}

	EditorCursor::Render();

	Renderer::EndScene();

	static Ref<Framebuffer> brightnessThreshold = CreateRef<Framebuffer>(framebuffer->width, framebuffer->height, true);
	Renderer::BrightnessHighPass(framebuffer, brightnessThreshold, 0.8f);

	static Ref<Framebuffer> copy1 = CreateRef<Framebuffer>(framebuffer->width / 2, framebuffer->height / 2, true);
	Renderer::Copy(brightnessThreshold, copy1);

	static Ref<Framebuffer> copy2 = CreateRef<Framebuffer>(copy1->width / 2, copy1->height / 2, true);
	Renderer::Copy(copy1, copy2);

	static Ref<Framebuffer> copy3 = CreateRef<Framebuffer>(copy2->width / 2, copy2->height / 2, true);
	Renderer::Copy(copy2, copy3);

	static Ref<Framebuffer> up1 = CreateRef<Framebuffer>(framebuffer->width, framebuffer->height, true);
	Renderer::Add(copy3, copy2, up1);

	static Ref<Framebuffer> up2 = CreateRef<Framebuffer>(framebuffer->width, framebuffer->height, true);
	Renderer::Add(up1, copy1, up2);

	static Ref<Framebuffer> up3 = CreateRef<Framebuffer>(framebuffer->width, framebuffer->height, true);
	Renderer::Add(up2, framebuffer, up3);

	Cockroach::Window& window = Application::Get().GetWindow();
	Renderer::OnWindowResize(window.width, window.height);

	static Ref<Framebuffer> displayBuf = up3;
	if (Input::IsDown(CR_KEY_J))
		displayBuf = up1;
	if (Input::IsDown(CR_KEY_K))
		displayBuf = up2;
	if (Input::IsDown(CR_KEY_L))
		displayBuf = up3;
	if (Input::IsDown(CR_KEY_N))
		displayBuf = framebuffer;
	if (Input::IsDown(CR_KEY_Y))
		displayBuf = copy1;
	if (Input::IsDown(CR_KEY_U))
		displayBuf = copy2;
	if (Input::IsDown(CR_KEY_I))
		displayBuf = copy3;

	if (!editMode)
	{
		//framebuffer->Unbind();
		Renderer::BlitToScreen(displayBuf);

		Application::ImGuiBegin();
		ExampleGameUI();
		Application::ImGuiEnd();
	}

	if(editMode)
		ImGuiRender();
}

void Game::ExampleGameUI()
{
	using namespace ImGui;

	ImGuiIO& io = ImGui::GetIO();
	PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 255).Value);
	PushStyleColor(ImGuiCol_FrameBg, ImColor(0, 0, 0, 0).Value);
	PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
	PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
	SetNextWindowSizeConstraints({ 400,150 }, { 400,150 }, nullptr, nullptr);
	Begin("Dialogue Box", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

	static int frame = 0;
	frame++;

	std::string strings[] = {u8"止クヨリ将末ワケシア国写3黒シ年示らとゃゆ国及謙紙む村満でょせ転百五トにゅ投譲セムケ人化ムミケ徽庭さお批会イセ意遅ム考32昨丞云みさげ。要メヤアク市89宜おせょば果誕んへドフ賞先らっ覚字あま省海倉ドフゃ占万のてっま裁動庭ラシイセ読合念じ歳左わスず通刊たさ後角べたか滑要エケカヨ資下の。",
	u8"代ヨオルユ愛生タヒヌ障業われ勝布らばり懲撃だかぞ写聞残的ヤホ臓月モアヌス縦掲ゃる川込チタツユ索図眺語ぐレぶべ。用ゆ出所うゆち写戦イヌ雪疑部ぶ南21人カハケ仲解ぎげ下難えスそり歩社ヲ安農メワ初器みきごゆ造期ソスシ訪2廃うぞろ。本すごきに韓姿ヘウヒマ自綱ちの情荘備ねッ教併そ南91会ど転休夕ラスレエ質振てゅくリ供以熟ナマヒネ治和しンあを知85芸ヌムモヱ数済ネキテイ情質又娠ぴごや。",
	u8"目ぞル村毒リエ的入ぐに新港ょがッね生要69来セオミ覧装ぼレ由藤紙ぎじべし載論ルツ患82敗案ス新治高第ルリ関災サソチ電塁ヱレヒ闘言集知ンっさ。梨ら様人リ略再ハミヘ言月ち上必ホ立削げろリ矢格こ護都リぼ宝明韓ヲカ謙権レ楽真とすぱ物止エウヒ表32道地テチトフ気日ぎ止性ぼほ院少ルヒヲ地愛鉄たう。",
	u8"査ミムモタ米議ロ済都羅ぜな還詳されの訴結ほめ調強トカオ年敗ホ物石べ画主ど共書なきみず視8殉立シ廃支う。必づそク公広つはまフ博障セワエ会旅ラ北6予リをろれ図疑フゅ連会ケハオ台検づたク豊少かわきば一質進ルん日力ぴご真償ー。任トニオ個中ニエ変表28自第ヘ護5意ほ思自キワ師願うー生赤違ニ馬前ヌラサモ電横月ヘルヌヒ引見芸ぜひん淑進券企レうぽ。",
	u8"負ラシ価科れぎつ教著けよせ水局岡ヌア経作れらのへ入観こがド改験スフソヌ古意ゅぎ秋空づ通表代段続メモユテ率幇だ満象ヨレコチ願減ごむさ利期見察棄銃ぽほえさ。故ヒマワ投作づぐれ部王イエマオ会48購活クフセ秋有の見15行ぎどぞル芸共12見ス感参ラチオヱ連設需誕へた。留ちとドッ囲注日ヨルミマ手供であリぽ歩徴スメヱ良代き日気ッ館業エ作分9属メ追9既杉トン。",
	u8"契よ日6必せぽみ事進ずびが点解かスきフ薬営速ょよーひ暮存ヱト誇崎ツカム為争ハフ済6度せぐも更市コフム新丁ソエテ竹終ノヨコ児読テタソ雄録傑摯フ。論ノフ全状ヨリ怖理さゅ分79面わーぐ近団気つぶ国井ぴ備掲ろびめラ虹野フ踊負シ式37社マタ疑表カサセ前府レょ。際わルう手談済さず百不業ワシスム柄53突なぐぴと見食をすと堅典回叩奇さとフは宅浩ム空館ちろげ融難キチア板北競役人述イド。",
	};

	int arrSize = sizeof(strings) / sizeof(strings[0]);

	TextWrapped(strings[(frame/6)%arrSize].c_str());

	End();
	PopStyleVar(2);
	PopStyleColor(2);
}

void Game::ImGuiRender()
{
	using namespace ImGui;
	Application::ImGuiBegin();

	ImGuiIO& io = ImGui::GetIO();
	Begin("Info");
	Text("%.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	Text("Pos: %i %i", player->position.x, player->position.y);
	Text("Vel: %.1f, %.1f", player->velocity.x, player->velocity.y);

	Checkbox("Hitboxes", &renderHitboxes);
	Checkbox("Grid", &renderGrid);
	Checkbox("Rooms", &renderAllRooms);
	Checkbox("Room Boundaries", &renderRoomBoundaries);

	bool modified = true;

	if (Checkbox("Mute Master", &data.mutedMaster))
		Audio::ToggleSoundMaster(data.mutedMaster);
	else if (Checkbox("Mute SFX", &data.mutedSFX))
		Audio::ToggleSoundSFX(data.mutedSFX);
	else if (Checkbox("Mute Music", &data.mutedMusic))
		Audio::ToggleSoundMusic(data.mutedMusic);
	else modified = false;

	if(modified) data.Save();

	End();

	ExampleGameUI();

	Begin("Current Room");
	static char nameBuffer[255];
	if (strcmp(nameBuffer, Room::current->name.c_str()))
		strcpy_s(nameBuffer, 255, Room::current->name.c_str());

	InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer));
	
	if (strcmp(nameBuffer, Room::current->name.c_str()))
		Room::current->Rename(nameBuffer);

	int2 roomPositionBeforeEdit = Room::current->position;
	InputInt("X", &Room::current->position.x, 8, 8);
	InputInt("Y", &Room::current->position.y, 8, 8);
	if (Room::current->position != roomPositionBeforeEdit)
	{
		int2 delta = Room::current->position - roomPositionBeforeEdit;
		for (auto& entity : Room::current->entities)
			entity->position += delta*8;
		Room::current->Save();
	}

	int2 size = { Room::current->width, Room::current->height };
	InputInt("W", &size.x, 1, 1);
	InputInt("H", &size.y, 1, 1);
	if (size != int2(Room::current->width, Room::current->height))
		Room::current->Resize(size.x, size.y);

	if (Button("Save"))
		Room::current->Save();
	SameLine();
	if (Button("Save All"))
		for (auto& room : rooms)
			room->Save();

	End();

	Begin("Brush Settings");
	if (BeginTabBar("Brush Mode"))
	{
		if (BeginTabItem("Tile"))
		{
			EditorCursor::brushMode = EditorCursor::BrushMode::Tile;
			static bool isBackground = false;
			if (Checkbox("Background", &isBackground))
				EditorCursor::tileType = isBackground ? Room::BackgroundBasic : Room::TileBasic;
			EndTabItem();
		}
		if (BeginTabItem("Entity"))
		{
			EditorCursor::brushMode = EditorCursor::BrushMode::Entity;

			if (EditorCursor::selectedEntity)
				EditorCursor::selectedEntity->RenderInspectorUI();

			if (BeginListBox("", GetContentRegionAvail()))
			{
				for (int i = EntityType::Particles + 1; i < EntityType::END; i++)
					if (Selectable(entityTypeNames[i].c_str(), EditorCursor::entityType == i))
						EditorCursor::entityType = i;
				EndListBox();
			}
			EndTabItem();
		}
		if (BeginTabItem("Decoration"))
		{
			EditorCursor::brushMode = EditorCursor::BrushMode::Decoration;
			static bool showSpriteEditor = false;

			if (Button("Add Decoration"))
				decorationSprites.push_back(Sprite(Game::baseSpriteSheet, ZERO, ONE));
			SameLine();
			if (Button("Open Sprite Editor"))
				showSpriteEditor = true;

			if (showSpriteEditor) ShowSpriteEditor(&showSpriteEditor);

			if (BeginListBox("", GetContentRegionAvail()))
			{
				ImGuiStyle& style = ImGui::GetStyle();
				float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
				for (int i = 0; i < decorationSprites.size(); i++)
				{
					ImGui::PushID(i);
					Sprite& decoration = decorationSprites[i];

					const float displaySize = 100;
					float2 size = { displaySize, displaySize };
					if (decoration.YSize() != 0) // Don't divide by zero
					{
						float aspect = decoration.XSize() / decoration.YSize();
						if (aspect > 1.0f) size.y = displaySize / aspect;
						else			   size.x = displaySize * aspect;
					}

					if (ImageButton(std::to_string(i).c_str(), (void*)(intptr_t)Game::baseSpriteSheet->rendererID, { size.x,size.y }, { decoration.min.x, decoration.max.y }, { decoration.max.x, decoration.min.y }))
					{
						EditorCursor::decorationType = i;
					}
					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + size.x; // Expected position if next button was on same line
					if (i + 1 < decorationSprites.size() && next_button_x2 < window_visible_x2)
						ImGui::SameLine();
					ImGui::PopID();
				}
				EndListBox();
			}
			EndTabItem();
		}
		if (BeginTabItem("Room"))
		{
			EditorCursor::brushMode = EditorCursor::BrushMode::Room;
			EndTabItem();
		}
		EndTabBar();
	}
	End();

	Application::ImGuiEnd();
}

void Game::ShowSpriteEditor(bool* open)
{
	// Derived from ImGui custom rendering canvas example.
	using namespace ImGui;
	ImGuiIO& io = ImGui::GetIO();
	Begin("Sprite Editor", open);

	Ref<Texture2D> texture = Game::baseSpriteSheet;
	const float spriteTileScale = 8.0f;
	const float canvasScale = 10.0f;
	static float2 rectStart, rectEnd;
	static float2 scrolling(0.0f, 0.0f);
	static bool boxSelecting = false;

	float2 canvasStart = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
	float2 canvasSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
	if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
	if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;
	float2 canvasEnd = float2(canvasStart.x + canvasSize.x, canvasStart.y + canvasSize.y);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled({ canvasStart.x, canvasStart.y }, { canvasEnd.x, canvasEnd.y }, IM_COL32(20, 20, 20, 255));
	draw_list->AddRect({ canvasStart.x, canvasStart.y }, { canvasEnd.x, canvasEnd.y }, IM_COL32(255, 255, 255, 255));

	ImGui::InvisibleButton("canvas", { canvasSize.x, canvasSize.y }, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool hovered = ImGui::IsItemHovered();
	const bool active = ImGui::IsItemActive();
	const float2 origin(canvasStart.x + scrolling.x, canvasStart.y + scrolling.y);
	const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
	float2 discreteMousePos = float2(mouse_pos_in_canvas.x - fmodf(mouse_pos_in_canvas.x, canvasScale), mouse_pos_in_canvas.y - fmodf(mouse_pos_in_canvas.y, canvasScale));

	if (hovered && !boxSelecting && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		rectStart = discreteMousePos;
		boxSelecting = true;
	}
	if (boxSelecting)
	{
		rectEnd = discreteMousePos;
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			boxSelecting = false;
		if (rectStart - rectEnd != ZERO)
		{
			float2 rectMin = { std::min(rectStart.x, rectEnd.x), std::min(rectStart.y, rectEnd.y) };
			float2 rectMax = { std::max(rectStart.x, rectEnd.x), std::max(rectStart.y, rectEnd.y) };

			float2 uvStart = float2(rectMin.x / canvasScale, texture->height - (rectMax.y / canvasScale)) / float2(texture->width, texture->height);
			float2 uvEnd = float2(rectMax.x / canvasScale, texture->height - (rectMin.y / canvasScale)) / float2(texture->width, texture->height);

			decorationSprites[EditorCursor::decorationType] = Sprite(texture, uvStart, uvEnd);
			SaveSprites();
		}
	}

	if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))
		scrolling += float2(io.MouseDelta.x, io.MouseDelta.y);

	draw_list->PushClipRect({ canvasStart.x, canvasStart.y }, { canvasEnd.x, canvasEnd.y }, true);
	float2 textureStart = origin;
	float2 textureEnd = origin + canvasScale * float2(texture->width, texture->height);
	draw_list->AddImage((void*)(intptr_t)texture->rendererID, { textureStart.x, textureStart.y }, { textureEnd.x, textureEnd.y }, { 0,1 }, { 1,0 });

	float GRID_STEP = canvasScale * spriteTileScale;
	for (float x = fmodf(scrolling.x, GRID_STEP); x < canvasSize.x; x += GRID_STEP)
		draw_list->AddLine({ canvasStart.x + x, canvasStart.y }, { canvasStart.x + x, canvasEnd.y }, IM_COL32(200, 200, 200, 40));
	for (float y = fmodf(scrolling.y, GRID_STEP); y < canvasSize.y; y += GRID_STEP)
		draw_list->AddLine({ canvasStart.x, canvasStart.y + y }, { canvasEnd.x, canvasStart.y + y }, IM_COL32(200, 200, 200, 40));
	draw_list->AddRectFilled({ origin.x + rectStart.x, origin.y + rectStart.y }, { origin.x + rectEnd.x, origin.y + rectEnd.y }, IM_COL32(0, 135, 0, 120));
	draw_list->AddRect({ origin.x + rectStart.x, origin.y + rectStart.y }, { origin.x + rectEnd.x, origin.y + rectEnd.y }, IM_COL32(0, 220, 0, 255));
	draw_list->PopClipRect();

	End();
}

void Game::RenderGrid()
{
	for (int i = -80; i < 80; i++)
	{
		int xFloor = (int)cameraController->camera.GetPosition().x;
		float xColor = ((i + xFloor) % 8 == 0) ? 0.8f : 0.4f;

		int yFloor = (int)cameraController->camera.GetPosition().y;
		float yColor = ((i + yFloor) % 8 == 0) ? 0.8f : 0.4f;

		Cockroach::Renderer::DrawLine({ i + xFloor, -80.0f + yFloor, 5.0f }, { i + xFloor, 79.0f + yFloor, 5.0f }, { xColor, xColor, xColor, 0.5f });
		Cockroach::Renderer::DrawLine({ -80.0f + xFloor, i + yFloor, 5.0f }, { 79.0f + xFloor, i + yFloor, 5.0f }, { yColor, yColor, yColor, 0.5f });
	}
}

void Game::RenderHitboxes()
{
	Renderer::DrawQuadOutline((float)player->Left(), (float)player->Right(), (float)player->Bottom(), (float)player->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });

	for (auto& ent : Room::current->entities)
	{
		if (EditorCursor::brushMode == EditorCursor::BrushMode::Decoration)
		{
			Decoration* deco = ent->As<Decoration>();
			if(deco)
				Renderer::DrawQuadOutline((float)deco->Left(), (float)deco->Right(), (float)deco->Bottom(), (float)deco->Top(), CYAN);
		}
		Dynamic* dyn = ent->As<Dynamic>();
		if (dyn)
			Renderer::DrawQuadOutline((float)dyn->Left(), (float)dyn->Right(), (float)dyn->Bottom(), (float)dyn->Top(), RED);
	}

	for (int i = 0; i < Room::current->width * Room::current->height; i++)
	{
		if (Room::current->tiles[i] == Room::TileBasic)
		{
			float2 worldPos = Room::current->RoomToWorldPosition(Room::current->IndexToRoomPosition(i));
			Renderer::DrawQuadOutline(worldPos.x, worldPos.x + 8.0f, worldPos.y, worldPos.y + 8.0f, RED);
		}
	}
}

void Game::Freeze(int frames)
{
	freezeTimer = Timer(frames);
}

void Game::SaveSprites()
{
	std::string filepath = "assets/textures/sprites.txt";
	std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!out) CR_CORE_ERROR("Could not open file '{0}'", filepath);
	
	for (int i = 0; i < entitySprites.size(); i++)
	{
		out << GenerateProperty("E", i);
		out << GenerateProperty("X0", entitySprites[i].min.x);
		out << GenerateProperty("Y0", entitySprites[i].min.y);
		out << GenerateProperty("X1", entitySprites[i].max.x);
		out << GenerateProperty("Y1", entitySprites[i].max.y);
		out << '\n';
	}
	for (int i = 0; i < decorationSprites.size(); i++)
	{
		out << GenerateProperty("D", i);
		out << GenerateProperty("X0", decorationSprites[i].min.x);
		out << GenerateProperty("Y0", decorationSprites[i].min.y);
		out << GenerateProperty("X1", decorationSprites[i].max.x);
		out << GenerateProperty("Y1", decorationSprites[i].max.y);
		out << '\n';
	}
	
	out.close();
}

void Game::LoadSprites()
{
	std::string filepath = "assets/textures/sprites.txt";
	std::fstream in(filepath, std::ios::in || std::ios::binary);
	if (!in) CR_CORE_ERROR("Could not open file '{0}'", filepath);

	std::string line;

	while (std::getline(in, line))
	{
		std::stringstream stream(line);
		int type;

		float2 min = { GetProperty<float>(stream, "X0"), GetProperty<float>(stream, "Y0") };
		float2 max = { GetProperty<float>(stream, "X1"), GetProperty<float>(stream, "Y1") };
		Sprite sprite = Sprite(Game::baseSpriteSheet, min, max);

		if (HasProperty(stream, "E"))
		{
			type = GetProperty<int>(stream, "E");
			if (entitySprites.size() < type + 1) 
				entitySprites.resize(type + 1);
			entitySprites[type] = sprite;
		}
		else if(HasProperty(stream, "D"))
		{
			type = GetProperty<int>(stream, "D");
			if (decorationSprites.size() < type + 1) 
				decorationSprites.resize(type + 1);
			decorationSprites[type] = sprite;
		}
	}
	in.close();
}

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Game();
}