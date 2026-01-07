import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.io.*;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Comparator; 

public class MazeApp extends JFrame {

    // --- AYARLAR ---
    private static final int ROWS = 20; 
    private static final int COLS = 20;
    private static final int MAX_CELLS = ROWS * COLS;
    
    // HIZ VE BEKLEME AYARLARI
    private static final int ANIM_SPEED_MS = 10;   // Animasyon hızı
    private static final int TEST_WAIT_MS = 1000;  // Testler arası bekleme (ms)

    // --- DOSYA KAYIT YOLU 
    private static final String SAVE_FOLDER_PATH = "../docs/SavedTests";

    // --- RENKLER ---
    private static final Color COLOR_BG = new Color(30, 30, 35); 
    private static final Color COLOR_PANEL = new Color(40, 44, 50);
    private static final Color COLOR_WALL = new Color(60, 60, 65);
    private static final Color COLOR_VISITED = new Color(0, 190, 240); // Cyan
    private static final Color COLOR_PATH = new Color(255, 100, 50);   // Turuncu
    private static final Color COLOR_EMPTY = new Color(245, 245, 250); // Beyaz
    private static final Color COLOR_HEAVY = new Color(155, 89, 182);  // Mor (Ağır Yol)
    private static final Color COLOR_START = new Color(46, 204, 113);  // Yeşil
    private static final Color COLOR_END = new Color(231, 76, 60);     // Kırmızı

    // --- BİLEŞENLER ---
    private JPanel mazeCanvas;
    private JTextArea resultsArea;
    private Timer animationTimer;
    private JCheckBox chkWeighted, chkDynamic, chkMulti;
    private DefaultListModel<TestResultData> historyModel;
    private JList<TestResultData> historyList;

    // --- HARİTA VERİLERİ ---
    private long currentSeed = System.currentTimeMillis();
    private List<Point> connections = new ArrayList<>();      
    private List<Point> heavyConnections = new ArrayList<>(); 
    
    // --- OTURUM VERİLERİ ---
    private List<TestResultData> sessionTests = new ArrayList<>();
    
    // --- ANİMASYON ---
    private TestResultData currentAnimatingTest = null;
    private List<Integer> animVisits = new ArrayList<>();
    private List<Integer> animPath = new ArrayList<>();
    
    // YOL HARİTASI (Hangi hücreler yolun parçası? O(1) erişim için)
    private boolean[] isPathCell = new boolean[MAX_CELLS]; 
    
    private int animIndex = 0;
    private int pathIndex = 0;
    private Queue<TestResultData> replayQueue = new LinkedList<>();
    private int matrixRowCounter = 0;

    public MazeApp() {
        setTitle("Advanced Maze Generator Solver ");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        try { UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName()); } catch (Exception e) {}

        setLayout(new BorderLayout(0, 0));
        getContentPane().setBackground(COLOR_BG);

        add(createSidebar(), BorderLayout.WEST);
        
        mazeCanvas = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g); 
                drawMazeResponsive(g);   
            }
        };
        mazeCanvas.setBackground(COLOR_BG);
        add(mazeCanvas, BorderLayout.CENTER);
        
        add(createBottomPanel(), BorderLayout.SOUTH);

        setSize(1350, 900); 
        setLocationRelativeTo(null); 
    }

    // --- ÇİZİM MOTORU ---
    private void drawMazeResponsive(Graphics g) {
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        int panelW = mazeCanvas.getWidth();
        int panelH = mazeCanvas.getHeight();
        int cellW = (panelW - 40) / COLS;
        int cellH = (panelH - 40) / ROWS;
        int cellSize = Math.max(Math.min(cellW, cellH), 4); 
        int startX = (panelW - cellSize * COLS) / 2;
        int startY = (panelH - cellSize * ROWS) / 2;

        // 0. Zemin
        g2.setColor(COLOR_WALL);
        g2.fillRect(startX, startY, cellSize * COLS, cellSize * ROWS);

        if (connections.isEmpty() && heavyConnections.isEmpty()) return;

        // 1. Hücreler
        for (int i = 0; i < MAX_CELLS; i++) drawCell(g2, i, startX, startY, cellSize, COLOR_EMPTY);

        // 2. Normal Yollar
        g2.setColor(COLOR_EMPTY); 
        for (Point p : connections) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_EMPTY);
        
        // 3. Ağır Yollar (Sadece içini boya)
        Color heavyColor = chkWeighted.isSelected() ? COLOR_HEAVY : COLOR_EMPTY;
        g2.setColor(heavyColor);
        for(Point p:heavyConnections){
            connectCells(g2, p.x, p.y, startX, startY, cellSize, heavyColor);
            drawCell(g2, p.x, startX, startY, cellSize, heavyColor); 
            drawCell(g2, p.y, startX, startY, cellSize, heavyColor);
        }

        // 4. Animasyon (Keşif)
        g2.setColor(COLOR_VISITED);
        for (Integer cellId : animVisits) drawCell(g2, cellId, startX, startY, cellSize, COLOR_VISITED);
        for (Point p : connections) checkAndDrawLine(g2, p, animVisits, startX, startY, cellSize, COLOR_VISITED);
        for (Point p : heavyConnections) checkAndDrawLine(g2, p, animVisits, startX, startY, cellSize, COLOR_VISITED);

        // 5. Final Yol (Topoloji Tabanlı - Kesintisiz)
        if (!animPath.isEmpty()) {
            g2.setColor(COLOR_PATH);
            for (Integer cellId : animPath) {
                drawCell(g2, cellId, startX, startY, cellSize, COLOR_PATH);
            }
            // Yol bağlantılarını zorla çiz (Duvar olsa bile üstüne)
            for (Point p : connections) {
                if (isPathCell[p.x] && isPathCell[p.y]) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_PATH);
            }
            for (Point p : heavyConnections) {
                if (isPathCell[p.x] && isPathCell[p.y]) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_PATH);
            }
        }

        drawTargets(g2, startX, startY, cellSize);
    }
    
    private void checkAndDrawLine(Graphics2D g, Point p, List<Integer> list, int sx, int sy, int size, Color c) {
        if(list.contains(p.x) && list.contains(p.y)) connectCells(g, p.x, p.y, sx, sy, size, c);
    }

    private void drawTargets(Graphics2D g2, int startX, int startY, int cellSize) {
        drawCell(g2, 0, startX, startY, cellSize, COLOR_START);
        g2.setColor(Color.WHITE); g2.setFont(new Font("Arial", Font.BOLD, 14)); g2.drawString("S", startX + cellSize/2 - 5, startY + cellSize/2 + 5);
        
        int total = ROWS * COLS;
        if (chkMulti.isSelected()) {
            drawT(g2, COLS - 1, "1", startX, startY, cellSize);
            drawT(g2, (ROWS - 1) * COLS, "2", startX, startY, cellSize);
            drawT(g2, total - 1, "E", startX, startY, cellSize);
        } else {
            drawT(g2, total - 1, "E", startX, startY, cellSize);
        }
    }
    private void drawT(Graphics2D g, int id, String s, int sx, int sy, int z) { 
        drawCell(g, id, sx, sy, z, COLOR_END); g.setColor(Color.WHITE); g.drawString(s, sx+(id%COLS)*z+z/2-4, sy+(id/COLS)*z+z/2+4); 
    }
    private void drawCell(Graphics2D g2, int cellId, int startX, int startY, int size, Color c) {
        if (cellId < 0 || cellId >= MAX_CELLS) return; 
        int r = cellId / COLS; int col = cellId % COLS;
        g2.setColor(c);
        g2.fillRoundRect(startX + col * size + 2, startY + r * size + 2, size - 4, size - 4, 8, 8);
    }
    private void connectCells(Graphics2D g2, int u, int v, int startX, int startY, int size, Color c) {
        if (u >= MAX_CELLS || v >= MAX_CELLS) return;
        int r1 = u / COLS, c1 = u % COLS; int r2 = v / COLS, c2 = v % COLS;
        g2.setColor(c); 
        int bridgeSize = size - 4;
        if (r1 == r2) { 
            int c_min = Math.min(c1, c2);
            g2.fillRect(startX + (c_min + 1) * size - 4, startY + r1 * size + 2, 8, bridgeSize);
        } else { 
            int r_min = Math.min(r1, r2);
            g2.fillRect(startX + c1 * size + 2, startY + (r_min + 1) * size - 4, bridgeSize, 8);
        }
    }

    // --- GUI ---
    private JPanel createSidebar() {
        JPanel sidebar = new JPanel(); sidebar.setLayout(new BoxLayout(sidebar, BoxLayout.Y_AXIS));
        sidebar.setBackground(COLOR_PANEL); sidebar.setPreferredSize(new Dimension(240, 0)); sidebar.setBorder(new EmptyBorder(15, 15, 15, 15));
        
        addLabel(sidebar, "AYARLAR");
        chkWeighted = createCheckbox("Ağırlıklı"); sidebar.add(chkWeighted);
        chkDynamic = createCheckbox("Dinamik Duvarlar"); sidebar.add(chkDynamic);
        chkMulti = createCheckbox("Çoklu Hedef"); sidebar.add(chkMulti);
        sidebar.add(Box.createVerticalStrut(20));

        addLabel(sidebar, "LABİRENT");
        JButton btnGen = createButton("YENİ HARİTA", new Color(41, 128, 185));
        btnGen.addActionListener(e -> generateMapAction());
        sidebar.add(btnGen);
        sidebar.add(Box.createVerticalStrut(10));
        
        JButton btnLoad = createButton("TEST YÜKLE", new Color(230, 126, 34));
        btnLoad.addActionListener(e -> loadJsonTest());
        sidebar.add(btnLoad);
        sidebar.add(Box.createVerticalStrut(30));

        addLabel(sidebar, "TEST ÇALIŞTIR");
        String[] algos = {"BFS", "DFS", "Dijkstra", "A*"};
        for (String algo : algos) {
            JButton btn = createButton(algo, new Color(39, 174, 96));
            btn.addActionListener(e -> runSolver(algo));
            sidebar.add(btn); sidebar.add(Box.createVerticalStrut(10));
        }
        return sidebar;
    }

    private JPanel createBottomPanel() {
        JPanel mainPanel = new JPanel(new BorderLayout());
        mainPanel.setBackground(new Color(25, 25, 30));
        mainPanel.setBorder(new EmptyBorder(5, 10, 10, 10));

        JPanel legendPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 20, 5));
        legendPanel.setBackground(new Color(35, 35, 40));
        legendPanel.setBorder(BorderFactory.createMatteBorder(0,0,1,0, Color.GRAY));
        legendPanel.add(createLegendItem(COLOR_START, "S"));
        legendPanel.add(createLegendItem(COLOR_END, "E"));
        legendPanel.add(createLegendItem(COLOR_VISITED, "Keşif"));
        legendPanel.add(createLegendItem(COLOR_PATH, "Yol"));
        legendPanel.add(createLegendItem(COLOR_HEAVY, "Ağır")); 
        mainPanel.add(legendPanel, BorderLayout.NORTH);

        JPanel contentPanel = new JPanel(new GridLayout(1, 2, 10, 0));
        contentPanel.setBackground(new Color(25, 25, 30));
        contentPanel.setPreferredSize(new Dimension(0, 180));

        resultsArea = new JTextArea(); resultsArea.setBackground(new Color(20, 20, 25)); resultsArea.setForeground(new Color(46, 204, 113));
        resultsArea.setFont(new Font("Consolas", Font.PLAIN, 13)); resultsArea.setEditable(false);
        JScrollPane scrollLog = new JScrollPane(resultsArea);
        scrollLog.setBorder(createTitle("CANLI LOG"));

        JPanel rightPanel = new JPanel(new BorderLayout()); rightPanel.setBackground(new Color(25, 25, 30));
        historyModel = new DefaultListModel<>(); 
        historyList = new JList<>(historyModel); 
        historyList.setBackground(new Color(35, 35, 40)); historyList.setForeground(Color.WHITE);
        
        historyList.setCellRenderer(new DefaultListCellRenderer() {
            @Override
            public Component getListCellRendererComponent(JList<?> list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
                Component c = super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
                c.setForeground(Color.WHITE);
                c.setBackground(isSelected ? new Color(60, 63, 65) : new Color(35, 35, 40));
                return c;
            }
        });
        
        JScrollPane scrollHist = new JScrollPane(historyList); scrollHist.setBorder(createTitle("OTURUM TESTLERİ"));
        
        JPanel btnPanel = new JPanel(new GridLayout(1, 4, 5, 0)); 
        btnPanel.setBackground(new Color(25, 25, 30));
        
        JButton btnSortStep = createSmallButton("Sırala: Adım");
        btnSortStep.addActionListener(e -> sortHistory("step"));
        JButton btnSortWeight = createSmallButton("Sırala: Ağırlık");
        btnSortWeight.addActionListener(e -> sortHistory("weight"));
        JButton btnSave = createSmallButton("TÜMÜNÜ KAYDET");
        btnSave.addActionListener(e -> saveSessionJson());
        JButton btnOpenDir = createSmallButton("KLASÖRÜ AÇ");
        btnOpenDir.addActionListener(e -> openSaveDir());

        btnPanel.add(btnSortStep); btnPanel.add(btnSortWeight);
        btnPanel.add(btnSave); btnPanel.add(btnOpenDir);
        
        rightPanel.add(scrollHist, BorderLayout.CENTER);
        rightPanel.add(btnPanel, BorderLayout.SOUTH);

        contentPanel.add(scrollLog); contentPanel.add(rightPanel);
        mainPanel.add(contentPanel, BorderLayout.CENTER);
        return mainPanel;
    }

    // --- AKSİYONLAR ---
    private void generateMapAction() { 
        currentSeed = System.currentTimeMillis();
        stopAnimation();
        connections.clear(); heavyConnections.clear();
        sessionTests.clear(); historyModel.clear();
        resultsArea.setText("> Yeni Labirent Oluşturuluyor... (Seed: " + currentSeed + ")\n");
        resetAnimData();
        mazeCanvas.repaint();
        new Thread(() -> executeBackend("BFS", 0)).start(); 
    }
    
    private void runSolver(String algo) {
        for(TestResultData t : sessionTests) {
            if(t.algoName.equalsIgnoreCase(algo)) {
                resultsArea.append("> " + algo + " (Replay)...\n");
                stopAnimation();
                playSingleTestAnimation(t);
                return;
            }
        }
        stopAnimation();
        resetAnimData();
        resultsArea.append("> Başlatılıyor: " + algo + "\n");
        new Thread(() -> executeBackend(algo, 1)).start();
    }

    private void stopAnimation() {
        if(animationTimer != null) animationTimer.stop();
        replayQueue.clear();
    }
    
    private void resetAnimData() {
        animVisits.clear(); animPath.clear();
        isPathCell = new boolean[MAX_CELLS];
        animIndex = 0; pathIndex = 0; matrixRowCounter = 0;
    }

    // --- BACKEND İLETİŞİMİ ---
    private void executeBackend(String algo, int mode) {
        try {
            ProcessBuilder pb = new ProcessBuilder("Demo.exe", algo);
            pb.redirectErrorStream(true);
            Process p = pb.start();
            PrintWriter pw = new PrintWriter(new OutputStreamWriter(p.getOutputStream()), true);
            
            String inputData = String.format("%d %d %d %d %d %d %d", 
                ROWS, COLS, chkWeighted.isSelected()?1:0, chkDynamic.isSelected()?1:0, 
                chkMulti.isSelected()?1:0, currentSeed, mode);
            
            pw.println(inputData);
            pw.flush();

            BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line;
            
            TestResultData currentResult = (mode == 1) ? new TestResultData(algo) : null;

            while((line = br.readLine()) != null) {
                String fLine = line.trim();
                if (fLine.startsWith("DEBUG:")) {
                    SwingUtilities.invokeLater(() -> resultsArea.append(fLine + "\n"));
                } 
                else if(fLine.equals("MAP_READY")) {
                    SwingUtilities.invokeLater(() -> mazeCanvas.repaint());
                } 
                else {
                    parseLine(fLine, currentResult);
                }
            }
            
            if(mode == 1 && currentResult != null) {
                sessionTests.add(currentResult);
                SwingUtilities.invokeLater(() -> {
                    historyModel.addElement(currentResult); 
                    playSingleTestAnimation(currentResult);
                });
            }
            
        } catch(Exception e) { SwingUtilities.invokeLater(() -> resultsArea.append("HATA: " + e.getMessage() + "\n")); }
    }

    private void parseLine(String line, TestResultData data) {
        try {
            if(line.startsWith("MATRIX:")) { 
                String[] vals = line.split(":")[1].trim().split("\\s+");
                int u = matrixRowCounter;
                if (u >= MAX_CELLS) return; 
                for(int v=0; v<vals.length; v++) {
                    if (v >= MAX_CELLS) break; 
                    try {
                        int w = Integer.parseInt(vals[v]);
                        if(w == 1) connections.add(new Point(u, v));
                        else if(w >= 2) heavyConnections.add(new Point(u, v));
                    } catch (Exception e) {}
                }
                matrixRowCounter++;
            }
            else if(data != null) { 
                Matcher mSteps = Pattern.compile("STEPS:\\s*(\\d+)").matcher(line);
                if (mSteps.find()) data.steps = Integer.parseInt(mSteps.group(1));
                
                Matcher mWeight = Pattern.compile("WEIGHT:\\s*(\\d+)").matcher(line);
                if (mWeight.find()) data.weight = Integer.parseInt(mWeight.group(1));
                
                if(line.startsWith("DYNA:")) data.dynaLogs.add(line.split(":")[1].trim());
                
                if(line.startsWith("EXPLORED:")) {
                    String[] parts = line.substring(9).trim().split("\\s+");
                    for(String s : parts) if(!s.isEmpty()) data.explored.add(Integer.parseInt(s));
                }
                else if(line.startsWith("RESULT:")) {
                    String[] parts = line.substring(7).trim().split("\\s+");
                    for(String s : parts) if(!s.isEmpty()) data.path.add(Integer.parseInt(s));
                }
            }
        } catch(Exception e) {}
    }

    // --- ANİMASYON ---
    private void playSingleTestAnimation(TestResultData test) {
        resetAnimData();
        currentAnimatingTest = test;
        resultsArea.append(">> Oynatılıyor: " + test.algoName + "\n");
        if (test.explored.isEmpty()) return;
        
        animationTimer = new Timer(ANIM_SPEED_MS, e -> {
            // 1. Keşif Aşaması (Her tıkta 2 kare - Hız ayarı)
            for(int i=0; i<2; i++) {
                if(animIndex < test.explored.size()) {
                    animVisits.add(test.explored.get(animIndex++));
                    // Dinamik duvar
                    if (animIndex % 10 == 0 && !test.dynaLogs.isEmpty()) {
                        processDynamicWallSafe(test.dynaLogs.remove(0), test);
                    }
                } else break;
            }
            
            // 2. Yol Aşaması (Keşif bitince)
            if(animIndex >= test.explored.size()) {
                if(pathIndex < test.path.size()) {
                    int cell = test.path.get(pathIndex++);
                    animPath.add(cell);
                    // YOL HARİTASINI GÜNCELLE
                    if(cell >=0 && cell < MAX_CELLS) isPathCell[cell] = true;
                } else {
                    ((Timer)e.getSource()).stop();
                    // BEKLEME VE KUYRUK
                    if(!replayQueue.isEmpty()) {
                        resultsArea.append(">> Bekleniyor (" + (TEST_WAIT_MS/1000) + "sn)...\n");
                        Timer delay = new Timer(TEST_WAIT_MS, evt -> {
                            ((Timer)evt.getSource()).stop();
                            playSingleTestAnimation(replayQueue.poll());
                        });
                        delay.setRepeats(false);
                        delay.start();
                    }
                }
            }
            mazeCanvas.repaint();
        });
        animationTimer.start();
    }

    private void processDynamicWallSafe(String data, TestResultData currentTest) {
        try {
            String[] vals = data.split("\\s+");
            int u = Integer.parseInt(vals[0]); int v = Integer.parseInt(vals[1]); int w = Integer.parseInt(vals[2]);
            if (w == 0 && isPartOfPath(u, v, currentTest.path)) return; // Koruma

            connections.removeIf(p -> (p.x == u && p.y == v) || (p.x == v && p.y == u));
            heavyConnections.removeIf(p -> (p.x == u && p.y == v) || (p.x == v && p.y == u));
            
            if (w == 1) { connections.add(new Point(u, v)); } 
            else if (w >= 2) { heavyConnections.add(new Point(u, v)); }
            
        } catch (Exception e) {}
    }

    private boolean isPartOfPath(int u, int v, List<Integer> path) {
        if(path == null || path.isEmpty()) return false;
        for(int i=0; i<path.size()-1; i++) {
            int p1 = path.get(i);
            int p2 = path.get(i+1);
            if ((p1 == u && p2 == v) || (p1 == v && p2 == u)) return true;
        }
        return false;
    }

    // --- KAYIT  ---
    private void saveSessionJson() {
        if (sessionTests.isEmpty()) {
            JOptionPane.showMessageDialog(this, "Kaydedilecek test yok!");
            return;
        }
        try {
            File dir = new File(SAVE_FOLDER_PATH);
            if (!dir.exists()) dir.mkdirs();
            String ts = new SimpleDateFormat("yyyyMMdd_HHmm").format(new Date());
            File file = new File(dir, "Session_" + ts + ".json");
            BufferedWriter w = new BufferedWriter(new FileWriter(file));
            
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"file_type\": \"MAZE_SOLVER_DATA\",\n");
            sb.append("  \"options\": { \"weight\": ").append(chkWeighted.isSelected()?1:0).append(" },\n");
            sb.append("  \"tests\": [\n");
            for(int k=0; k<sessionTests.size(); k++) {
                TestResultData t = sessionTests.get(k);
                sb.append("    {\n");
                sb.append("      \"name\": \"").append(t.algoName).append("\",\n");
                sb.append("      \"steps\": ").append(t.steps).append(",\n");
                sb.append("      \"weight\": ").append(t.weight).append(",\n");
                sb.append("      \"explored\": [");
                for(int i=0; i<t.explored.size(); i++) { sb.append(t.explored.get(i)); if(i<t.explored.size()-1) sb.append(","); }
                sb.append("],\n");
                sb.append("      \"path\": [");
                for(int i=0; i<t.path.size(); i++) { sb.append(t.path.get(i)); if(i<t.path.size()-1) sb.append(","); }
                sb.append("]\n");
                sb.append("    }");
                if(k < sessionTests.size()-1) sb.append(",");
                sb.append("\n");
            }
            sb.append("  ],\n");
            
            sb.append("  \"connections\": [");
            for(int i=0; i<connections.size(); i++) { sb.append("\"").append(connections.get(i).x).append(",").append(connections.get(i).y).append("\""); if(i<connections.size()-1) sb.append(","); }
            sb.append("],\n");
            sb.append("  \"heavy\": [");
            for(int i=0; i<heavyConnections.size(); i++) { sb.append("\"").append(heavyConnections.get(i).x).append(",").append(heavyConnections.get(i).y).append("\""); if(i<heavyConnections.size()-1) sb.append(","); }
            sb.append("]\n");

            sb.append("}");
            w.write(sb.toString());
            w.close();
            JOptionPane.showMessageDialog(this, "Oturum Kaydedildi:\n" + file.getName());
        } catch(IOException e) { JOptionPane.showMessageDialog(this, "Hata: " + e.getMessage()); }
    }

    // --- YÜKLEME  ---
    private void loadJsonTest() {
        JFileChooser fileChooser = new JFileChooser(SAVE_FOLDER_PATH);
        fileChooser.setFileFilter(new FileNameExtensionFilter("JSON Dosyaları", "json"));
        if (fileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                String content = new String(Files.readAllBytes(fileChooser.getSelectedFile().toPath()));
                
                stopAnimation();
                connections.clear(); heavyConnections.clear();
                sessionTests.clear(); historyModel.clear();
                resetAnimData();

                parseMapData(content, "\"connections\":\\s*\\[(.*?)\\]", connections);
                parseMapData(content, "\"heavy\":\\s*\\[(.*?)\\]", heavyConnections);
                
                mazeCanvas.repaint();

                int index = 0;
                while(true) {
                    int nameIdx = content.indexOf("\"name\":", index);
                    if(nameIdx == -1) break;
                    int endObjIdx = content.indexOf("}", nameIdx);
                    String block = content.substring(nameIdx, endObjIdx);
                    
                    TestResultData t = new TestResultData("Unknown");
                    Matcher mn = Pattern.compile("\"name\":\\s*\"(.*?)\"").matcher(block); if(mn.find()) t.algoName = mn.group(1);
                    Matcher ms = Pattern.compile("\"steps\":\\s*(\\d+)").matcher(block); if(ms.find()) t.steps = Integer.parseInt(ms.group(1));
                    Matcher mw = Pattern.compile("\"weight\":\\s*(\\d+)").matcher(block); if(mw.find()) t.weight = Integer.parseInt(mw.group(1));
                    
                    Matcher me = Pattern.compile("\"explored\":\\s*\\[(.*?)\\]").matcher(block);
                    if(me.find()) for(String s:me.group(1).split(",")) if(!s.trim().isEmpty()) t.explored.add(Integer.parseInt(s.trim()));
                    
                    Matcher mp = Pattern.compile("\"path\":\\s*\\[(.*?)\\]").matcher(block);
                    if(mp.find()) for(String s:mp.group(1).split(",")) if(!s.trim().isEmpty()) t.path.add(Integer.parseInt(s.trim()));
                    
                    sessionTests.add(t);
                    historyModel.addElement(t);
                    replayQueue.add(t);
                    index = endObjIdx + 1;
                }

                resultsArea.append("> Kayıt Yüklendi. Replay Başlıyor...\n");
                if(!replayQueue.isEmpty()) playSingleTestAnimation(replayQueue.poll());

            } catch (Exception e) { JOptionPane.showMessageDialog(this, "Yükleme Hatası: " + e.getMessage()); e.printStackTrace();}
        }
    }
    
    private void parseMapData(String content, String regex, List<Point> list) {
        Matcher mBlock = Pattern.compile(regex).matcher(content);
        if (mBlock.find()) {
            String arrayContent = mBlock.group(1);
            Matcher mPair = Pattern.compile("\"(\\d+)\\s*,\\s*(\\d+)\"").matcher(arrayContent);
            while (mPair.find()) {
                list.add(new Point(Integer.parseInt(mPair.group(1)), Integer.parseInt(mPair.group(2))));
            }
        }
    }

    // --- YARDIMCI SINIF ---
    class TestResultData {
        String algoName; int steps, weight;
        List<Integer> explored = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        List<String> dynaLogs = new ArrayList<>();
        public TestResultData(String name) { this.algoName = name; }
        @Override
        public String toString() { return String.format("[%s] S: %d | W: %d", algoName, steps, weight); }
    }

    private void sortHistory(String type) {
        List<TestResultData> l = new ArrayList<>();
        for(int i=0; i<historyModel.getSize(); i++) l.add(historyModel.get(i));
        if(type.equals("step")) l.sort(Comparator.comparingInt(o -> o.steps)); else l.sort(Comparator.comparingInt(o -> o.weight));
        historyModel.clear(); l.forEach(historyModel::addElement);
    }

    // Basit metodlar
    private JButton createButton(String text, Color bg) {
        JButton btn = new JButton(text); btn.setBackground(bg); btn.setForeground(Color.WHITE);
        btn.setFocusPainted(false); btn.setFont(new Font("Arial", Font.BOLD, 12));
        btn.setMaximumSize(new Dimension(Integer.MAX_VALUE, 40)); btn.setAlignmentX(Component.LEFT_ALIGNMENT); return btn;
    }
    private JButton createSmallButton(String text) {
        JButton btn = new JButton(text); btn.setFont(new Font("Arial", Font.BOLD, 11));
        btn.setBackground(Color.DARK_GRAY); btn.setForeground(Color.WHITE); btn.setFocusPainted(false); return btn;
    }
    private JCheckBox createCheckbox(String text) {
        JCheckBox chk = new JCheckBox(text); chk.setBackground(COLOR_PANEL); chk.setForeground(Color.WHITE);
        chk.setFocusPainted(false); chk.setAlignmentX(Component.LEFT_ALIGNMENT); return chk;
    }
    private void addLabel(JPanel p, String text) {
        JLabel l = new JLabel(text); l.setForeground(Color.CYAN); l.setFont(new Font("Arial", Font.BOLD, 12));
        l.setAlignmentX(Component.LEFT_ALIGNMENT); p.add(l); p.add(Box.createVerticalStrut(5));
    }
    private JPanel createLegendItem(Color c, String text) {
        JPanel p = new JPanel(new FlowLayout(FlowLayout.LEFT, 5, 0)); p.setOpaque(false);
        JPanel box = new JPanel(); box.setPreferredSize(new Dimension(12, 12)); box.setBackground(c);
        box.setBorder(BorderFactory.createLineBorder(Color.WHITE));
        JLabel l = new JLabel(text); l.setForeground(Color.LIGHT_GRAY); p.add(box); p.add(l); return p;
    }
    private TitledBorder createTitle(String t) {
        return BorderFactory.createTitledBorder(null, t, TitledBorder.LEFT, TitledBorder.TOP, new Font("Arial",Font.BOLD,12), Color.GRAY);
    }
    
    // --- KLASÖR AÇMA  ---
    private void openSaveDir() {
        try { File path = new File(SAVE_FOLDER_PATH); if(!path.exists()) path.mkdirs(); Desktop.getDesktop().open(path); } 
        catch (IOException e) { JOptionPane.showMessageDialog(this, "Klasör açılamadı."); }
    }

    public static void main(String[] args) { SwingUtilities.invokeLater(() -> new MazeApp().setVisible(true)); }
}