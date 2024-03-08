#include <Cockroach.h>
#include <Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EntityDef.h"
#include "Entities/Player.h"
#include "EditorCursor.h"

#include "Game.h"

using namespace Entities;

PlayerData Game::data;

CameraController* Game::cameraController = nullptr;
Player* Game::player = nullptr;
Entities::Particles* Game::particles = nullptr;

Ref<Framebuffer> Game::framebuffer = nullptr;
Ref<Framebuffer> Game::distortionFramebuffer = nullptr;
Ref<Texture2D> Game::baseSpriteSheet = nullptr;
Ref<Texture2D> Game::tilemapSheet = nullptr;
Ref<Texture2D> Game::background = nullptr;

vector<Sprite> Game::entitySprites;
vector<Sprite> Game::decorationSprites;

vector<Ref<Room>> Game::rooms;

bool Game::editMode = true;

Game::Game()
	: Application()
{
	Game::framebuffer = CreateRef<Framebuffer>(320, 180);
	Game::distortionFramebuffer = CreateRef<Framebuffer>(320, 180);
	Game::background = CreateRef<Texture2D>("assets/textures/BG_RED.png");
	Game::baseSpriteSheet = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
	Game::tilemapSheet = CreateRef<Texture2D>("assets/textures/Tilemaps.png");

	LoadSprites();

	Application::GetWindow().SetWindowMode(data.fullscreen);
	Audio::ToggleSoundMaster(data.mutedMaster);
	Audio::ToggleSoundSFX(data.mutedSFX);
	Audio::ToggleSoundMusic(data.mutedMusic);

	Input::actionMap[InputAction::Left]	 = data.actionKeys[0];
	Input::actionMap[InputAction::Right] = data.actionKeys[1];
	Input::actionMap[InputAction::Down]	 = data.actionKeys[2];
	Input::actionMap[InputAction::Up]	 = data.actionKeys[3];
	Input::actionMap[InputAction::Jump]  = data.actionKeys[4];
	Input::actionMap[InputAction::Dash]  = data.actionKeys[5];

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

	Game::player = new Player(data.playerPosition);
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
		string name = a.path().string().substr(a.path().string().find_last_of("/\\") + 1);
		rooms.push_back(Room::Load(name));
	}

	for (auto& room : rooms)
		if (room->Contains(player->WorldHitbox()))
			Room::current = room;

	static Sound sound = Sound("assets/audio/SheerIceTorrent.mp3", true);
	sound.Start();
}

void Game::Update()
{
	if (!freezeTimer.Finished())
	{
		return;
	}

	if (cameraController->isTransitioning) 
	{
		cameraController->Update();
		return;
	}

	if (!editMode)
	{
		player->Update();
		particles->Update();
	}

	if (Input::IsDown(CR_KEY_O))
	{
		timeScale += 1;
	}
	if (Input::IsDown(CR_KEY_P))
	{
		timeScale -= 1;
	}
	timeScale = max(0.0f, timeScale);
	
	Room::current->Update();
	cameraController->Update();

	EditorCursor::Update();

	if (saveTimer.Finished())
	{
		data.Save();
		saveTimer.Reset();
	}

	if (Input::IsDown(CR_KEY_TAB))
	{
		cameraController->SetZoom(90.0f);
		editMode = !editMode;
	}

	if (Input::IsDown(CR_KEY_F))
	{
		Application::GetWindow().SetWindowMode(!Application::GetWindow().fullscreen);
		data.fullscreen = Application::GetWindow().fullscreen;
	}
	if (Input::IsDown(CR_KEY_M))
	{
		Audio::ToggleSoundMaster(data.mutedMaster = !data.mutedMaster);
	}

	if (Input::IsDown(CR_KEY_G)) renderGrid = !renderGrid;
	if (Input::IsDown(CR_KEY_H)) renderHitboxes = !renderHitboxes;
	if (Input::IsDown(CR_KEY_R)) renderAllRooms = !renderAllRooms;
	if (Input::IsDown(CR_KEY_T)) showInfo = !showInfo;
}

global InputAction actionToRebind = InputAction::Left;
void RebindKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
	{
		return;
	}

	Input::actionMap[actionToRebind] = key;
	Game::data.actionKeys[actionToRebind] = key;

	Game::Get().GetWindow().ResetKeyCallback();
	Game::data.Save();

	timeScale = 1;
}

void Game::BeginKeyRebind(InputAction action)
{
	actionToRebind = action;
	glfwSetKeyCallback(GetWindow().GLFWWindow, RebindKeyCallback);
	timeScale = 0;
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
			rooms[i]->Render(Game::tilemapSheet, cameraController->Bounds());

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
			Rect r = ent->As<Turbine>()->turbineRect;
			Renderer::DrawQuadOutline(r.min.x, r.max.x, r.min.y, r.max.y, GREEN);
		}
	}

	EditorCursor::Render();

	Renderer::EndScene();

	static Ref<Framebuffer> copy1 = CreateRef<Framebuffer>(framebuffer->width / 2, framebuffer->height / 2, true);
	Renderer::BloomPrefilter(framebuffer, copy1, 0.3f);

	static Ref<Framebuffer> copy2 = CreateRef<Framebuffer>(copy1->width / 2, copy1->height / 2, true);
	Renderer::BloomDownsample(copy1, copy2);

	static Ref<Framebuffer> copy3 = CreateRef<Framebuffer>(copy2->width / 2, copy2->height / 2, true);
	Renderer::BloomDownsample(copy2, copy3);

	static Ref<Framebuffer> up1 = CreateRef<Framebuffer>(framebuffer->width, framebuffer->height, true);
	Renderer::BloomUpsample(copy3, copy2, up1);

	static Ref<Framebuffer> up2 = CreateRef<Framebuffer>(framebuffer->width, framebuffer->height, true);
	Renderer::BloomUpsample(up1, copy1, up2);

	static Ref<Framebuffer> up3 = CreateRef<Framebuffer>(framebuffer->width, framebuffer->height, false);
	Renderer::BloomUpsample(up2, framebuffer, up3);

	Cockroach::Window& window = Application::Get().GetWindow();
	Renderer::OnWindowResize(window.width, window.height);

	if (!editMode)
	{
		//framebuffer->Unbind();
		static Ref<Framebuffer> h = CreateRef<Framebuffer>(window.width, window.height, false);
		Renderer::Distortion(up3, distortionFramebuffer, h);
		Renderer::BlitToScreen(h);

		Application::ImGuiBegin();
		ExampleGameUI();
		if(showInfo) ShowExampleAppSimpleOverlay();
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

	string strings[] = {u8"止クヨリ将末ワケシア国写3黒シ年示らとゃゆ国及謙紙む村満でょせ転百五トにゅ投譲セムケ人化ムミケ徽庭さお批会イセ意遅ム考32昨丞云みさげ。要メヤアク市89宜おせょば果誕んへドフ賞先らっ覚字あま省海倉ドフゃ占万のてっま裁動庭ラシイセ読合念じ歳左わスず通刊たさ後角べたか滑要エケカヨ資下の。",
	u8"代ヨオルユ愛生タヒヌ障業われ勝布らばり懲撃だかぞ写聞残的ヤホ臓月モアヌス縦掲ゃる川込チタツユ索図眺語ぐレぶべ。用ゆ出所うゆち写戦イヌ雪疑部ぶ南21人カハケ仲解ぎげ下難えスそり歩社ヲ安農メワ初器みきごゆ造期ソスシ訪2廃うぞろ。本すごきに韓姿ヘウヒマ自綱ちの情荘備ねッ教併そ南91会ど転休夕ラスレエ質振てゅくリ供以熟ナマヒネ治和しンあを知85芸ヌムモヱ数済ネキテイ情質又娠ぴごや。",
	u8"目ぞル村毒リエ的入ぐに新港ょがッね生要69来セオミ覧装ぼレ由藤紙ぎじべし載論ルツ患82敗案ス新治高第ルリ関災サソチ電塁ヱレヒ闘言集知ンっさ。梨ら様人リ略再ハミヘ言月ち上必ホ立削げろリ矢格こ護都リぼ宝明韓ヲカ謙権レ楽真とすぱ物止エウヒ表32道地テチトフ気日ぎ止性ぼほ院少ルヒヲ地愛鉄たう。",
	u8"査ミムモタ米議ロ済都羅ぜな還詳されの訴結ほめ調強トカオ年敗ホ物石べ画主ど共書なきみず視8殉立シ廃支う。必づそク公広つはまフ博障セワエ会旅ラ北6予リをろれ図疑フゅ連会ケハオ台検づたク豊少かわきば一質進ルん日力ぴご真償ー。任トニオ個中ニエ変表28自第ヘ護5意ほ思自キワ師願うー生赤違ニ馬前ヌラサモ電横月ヘルヌヒ引見芸ぜひん淑進券企レうぽ。",
	u8"負ラシ価科れぎつ教著けよせ水局岡ヌア経作れらのへ入観こがド改験スフソヌ古意ゅぎ秋空づ通表代段続メモユテ率幇だ満象ヨレコチ願減ごむさ利期見察棄銃ぽほえさ。故ヒマワ投作づぐれ部王イエマオ会48購活クフセ秋有の見15行ぎどぞル芸共12見ス感参ラチオヱ連設需誕へた。留ちとドッ囲注日ヨルミマ手供であリぽ歩徴スメヱ良代き日気ッ館業エ作分9属メ追9既杉トン。",
	u8"契よ日6必せぽみ事進ずびが点解かスきフ薬営速ょよーひ暮存ヱト誇崎ツカム為争ハフ済6度せぐも更市コフム新丁ソエテ竹終ノヨコ児読テタソ雄録傑摯フ。論ノフ全状ヨリ怖理さゅ分79面わーぐ近団気つぶ国井ぴ備掲ろびめラ虹野フ踊負シ式37社マタ疑表カサセ前府レょ。際わルう手談済さず百不業ワシスム柄53突なぐぴと見食をすと堅典回叩奇さとフは宅浩ム空館ちろげ融難キチア板北競役人述イド。",
	u8"注政図見現携芸育潟風行生授行救。別今会薄一稿童難人紙会必生熱定己。松分事勢姿優馬住月農銅始末資。石向票将共質画際日究可書回人権神方。主王人半試一的九悪役文転上行定。金円裁今効候業合講受聖妹臨購注属迎感図。作計席訃周携学総社松国樹権火次治。員参工投亡村申総愛章康資送件承場方。敬悩体障効億第話犯芸歌腕裏禁。",
	u8"국회의원은 그 지위를 남용하여 국가·공공단체 또는 기업체와의 계약이나 그 처분에 의하여 재산상의 권리·이익 또는 직위를 취득하거나 타인을 위하여 그 취득을 알선할 수 없다. 모든 국민은 자기의 행위가 아닌 친족의 행위로 인하여 불이익한 처우를 받지 아니한다.국회는 정부의 동의없이 정부가 제출한 지출예산 각항의 금액을 증가하거나 새 비목을 설치할 수 없다."
	};

	int arrSize = sizeof(strings) / sizeof(strings[0]);

	TextWrapped(strings[(frameCount/165)%arrSize].c_str());


	End();
	PopStyleVar(2);
	PopStyleColor(2);
}

void Game::ShowExampleAppSimpleOverlay()
{
	static int location = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiTableFlags_NoBordersInBody;
	if (location >= 0)
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	else if (location == -2)
	{
		// Center window
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
	if (ImGui::Begin("Example: Simple overlay", &*(bool*)false, window_flags))
	{
		//IMGUI_DEMO_MARKER("Examples/Simple Overlay");
		ImGui::Text("%.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::Text("Pos: %i %i", player->position.x, player->position.y);
		ImGui::Text("Vel: %.1f, %.1f", player->velocity.x, player->velocity.y);
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
			ImGui::EndPopup();
		}
	}
	ImGui::PopStyleVar(1);
	ImGui::End();
}

struct ExampleAppConsole
{
	char                  InputBuf[256];
	ImVector<char*>       Items;
	ImVector<const char*> Commands;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter       Filter;
	bool                  AutoScroll;
	bool                  ScrollToBottom;

	ExampleAppConsole()
	{
		//IMGUI_DEMO_MARKER("Examples/Console");
		ClearLog();
		memset(InputBuf, 0, sizeof(InputBuf));
		HistoryPos = -1;

		// "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
		Commands.push_back("HELP");
		Commands.push_back("HISTORY");
		Commands.push_back("CLEAR");
		Commands.push_back("CLASSIFY");
		AutoScroll = true;
		ScrollToBottom = false;
		AddLog("Welcome to Dear ImGui!");
	}
	~ExampleAppConsole()
	{
		ClearLog();
		for (int i = 0; i < History.Size; i++)
			free(History[i]);
	}

	// Portable helpers
	static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
	static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
	static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

	void    ClearLog()
	{
		for (int i = 0; i < Items.Size; i++)
			free(Items[i]);
		Items.clear();
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		// FIXME-OPT
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		Items.push_back(Strdup(buf));
	}

	void    Draw(const char* title, bool* p_open)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
		// So e.g. IsItemHovered() will return true when hovering the title bar.
		// Here we create a context menu only available from the title bar.
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Close Console"))
				*p_open = false;
			ImGui::EndPopup();
		}

		ImGui::TextWrapped(
			"This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
			"implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
		ImGui::TextWrapped("Enter 'HELP' for help.");

		// TODO: display items starting from the bottom

		if (ImGui::SmallButton("Add Debug Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
		ImGui::SameLine();
		if (ImGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
		ImGui::SameLine();
		if (ImGui::SmallButton("Clear")) { ClearLog(); }
		ImGui::SameLine();
		bool copy_to_clipboard = ImGui::SmallButton("Copy");
		//static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

		ImGui::Separator();

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Options, Filter
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
		ImGui::Separator();

		// Reserve enough left-over height for 1 separator + 1 input text
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Clear")) ClearLog();
				ImGui::EndPopup();
			}

			// Display every line as a separate entry so we can change their color or add custom widgets.
			// If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
			// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
			// to only process visible items. The clipper will automatically measure the height of your first item and then
			// "seek" to display only items in the visible area.
			// To use the clipper we can replace your standard loop:
			//      for (int i = 0; i < Items.Size; i++)
			//   With:
			//      ImGuiListClipper clipper;
			//      clipper.Begin(Items.Size);
			//      while (clipper.Step())
			//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			// - That your items are evenly spaced (same height)
			// - That you have cheap random access to your elements (you can access them given their index,
			//   without processing all the ones before)
			// You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
			// We would need random-access on the post-filtered list.
			// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
			// or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
			// and appending newly elements as they are inserted. This is left as a task to the user until we can manage
			// to improve this example code!
			// If your items are of variable height:
			// - Split them into same height items would be simpler and facilitate random-seeking into your list.
			// - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			if (copy_to_clipboard)
				ImGui::LogToClipboard();
			for (int i = 0; i < Items.Size; i++)
			{
				const char* item = Items[i];
				if (!Filter.PassFilter(item))
					continue;

				// Normally you would store more information in your item than just a string.
				// (e.g. make Items[] an array of structure, store color/type etc.)
				ImVec4 color;
				bool has_color = false;
				if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
				else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
				if (has_color)
					ImGui::PushStyleColor(ImGuiCol_Text, color);
				ImGui::TextUnformatted(item);
				if (has_color)
					ImGui::PopStyleColor();
			}
			if (copy_to_clipboard)
				ImGui::LogFinish();

			if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
				ImGui::SetScrollHereY(1.0f);
			ScrollToBottom = false;

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
		ImGui::Separator();

		// Command-line
		bool reclaim_focus = false;
		ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
		{
			char* s = InputBuf;
			Strtrim(s);
			if (s[0])
				ExecCommand(s);
			strcpy(s, "");
			reclaim_focus = true;
		}

		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if (reclaim_focus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		ImGui::End();
	}

	void    ExecCommand(const char* command_line)
	{
		AddLog("# %s\n", command_line);

		// Insert into history. First find match and delete it so it can be pushed to the back.
		// This isn't trying to be smart or optimal.
		HistoryPos = -1;
		for (int i = History.Size - 1; i >= 0; i--)
			if (Stricmp(History[i], command_line) == 0)
			{
				free(History[i]);
				History.erase(History.begin() + i);
				break;
			}
		History.push_back(Strdup(command_line));

		// Process command
		if (Stricmp(command_line, "CLEAR") == 0)
		{
			ClearLog();
		}
		else if (Stricmp(command_line, "HELP") == 0)
		{
			AddLog("Commands:");
			for (int i = 0; i < Commands.Size; i++)
				AddLog("- %s", Commands[i]);
		}
		else if (Stricmp(command_line, "HISTORY") == 0)
		{
			int first = History.Size - 10;
			for (int i = first > 0 ? first : 0; i < History.Size; i++)
				AddLog("%3d: %s\n", i, History[i]);
		}
		else
		{
			AddLog("Unknown command: '%s'\n", command_line);
		}

		// On command input, we scroll to bottom even if AutoScroll==false
		ScrollToBottom = true;
	}

	// In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
	{
		ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
		return console->TextEditCallback(data);
	}

	int     TextEditCallback(ImGuiInputTextCallbackData* data)
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION

			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf)
			{
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				word_start--;
			}

			// Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < Commands.Size; i++)
				if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
					candidates.push_back(Commands[i]);

			if (candidates.Size == 0)
			{
				// No match
				AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
			}
			else if (candidates.Size == 1)
			{
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				// Multiple matches. Complete as much as we can..
				// So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
				int match_len = (int)(word_end - word_start);
				for (;;)
				{
					int c = 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0)
							c = toupper(candidates[i][match_len]);
						else if (c == 0 || c != toupper(candidates[i][match_len]))
							all_candidates_matches = false;
					if (!all_candidates_matches)
						break;
					match_len++;
				}

				if (match_len > 0)
				{
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
				}

				// List matches
				AddLog("Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++)
					AddLog("- %s\n", candidates[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
					HistoryPos = History.Size - 1;
				else if (HistoryPos > 0)
					HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size)
						HistoryPos = -1;
			}

			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
		}
		}
		return 0;
	}
};

static void ShowExampleAppConsole(bool* p_open)
{
	static ExampleAppConsole console;
	console.Draw("Example: Console", p_open);
}

void Game::ImGuiRender()
{
	using namespace ImGui;
	
	Application::ImGuiBegin();
	
	//bool consoleOpen = true;
	//if(consoleOpen) ShowExampleAppConsole(&consoleOpen); // Look at how this works and implement a similar debug/command console in the future.

	ImGuiIO& io = ImGui::GetIO();
	Begin("Settings");

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

	Text("Move Left currently binded to: %d", Input::actionMap[Left]);
	Text("Move Right currently binded to: %d", Input::actionMap[Right]);
	Text("Face Down currently binded to: %d", Input::actionMap[Down]);
	Text("Face Up currently binded to: %d", Input::actionMap[Up]);
	Text("Jump currently binded to: %d", Input::actionMap[Jump]);
	Text("Dash currently binded to: %d", Input::actionMap[InputAction::Dash]);

	if (Button("Rebind Move Left"))
	{
		ImGui::SetWindowFocus(nullptr);
		BeginKeyRebind(InputAction::Left);
	}
	if (Button("Rebind Move Right"))
	{
		ImGui::SetWindowFocus(nullptr);
		BeginKeyRebind(InputAction::Right);
	}
	if (Button("Rebind Face Down"))
	{
		ImGui::SetWindowFocus(nullptr);
		BeginKeyRebind(InputAction::Down);
	}
	if (Button("Rebind Face Up"))
	{
		ImGui::SetWindowFocus(nullptr);
		BeginKeyRebind(InputAction::Up);
	}
	if (Button("Rebind Jump"))
	{
		ImGui::SetWindowFocus(nullptr);
		BeginKeyRebind(InputAction::Jump);
	}
	if (Button("Rebind Dash"))
	{
		ImGui::SetWindowFocus(nullptr);
		BeginKeyRebind(InputAction::Dash);
	}

	End();

	ExampleGameUI();
	ShowExampleAppSimpleOverlay();

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
			float2 rectMin = { min(rectStart.x, rectEnd.x), min(rectStart.y, rectEnd.y) };
			float2 rectMax = { max(rectStart.x, rectEnd.x), max(rectStart.y, rectEnd.y) };

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
{   // Find a better way to do this...
	dynamic_cast<Game&>(Application::Get()).freezeTimer = Timer(frames, TimerType::frames);
}

void Game::SaveSprites()
{
	string filepath = "assets/textures/sprites.txt";
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
	string filepath = "assets/textures/sprites.txt";
	std::fstream in(filepath, std::ios::in || std::ios::binary);
	if (!in) CR_CORE_ERROR("Could not open file '{0}'", filepath);

	string line;

	while (std::getline(in, line))
	{
		stringstream stream(line);
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

Game::~Game()
{
	data.Save();
	CR_INFO("Game saved on close.");
}

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Game();
}