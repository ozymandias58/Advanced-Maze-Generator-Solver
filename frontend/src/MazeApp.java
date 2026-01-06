import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.io.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MazeApp extends JFrame {

    // --- AYARLAR ---
    private static final int ROWS = 20; 
    private static final int COLS = 20;

    // --- RENKLER (Profesyonel Palet) ---
    private static final Color COLOR_BG = new Color(30, 30, 35); 
    private static final Color COLOR_PANEL = new Color(40, 44, 50);
    private static final Color COLOR_WALL = new Color(60, 60, 65);
    private static final Color COLOR_VISITED = new Color(0, 190, 240); // Cyan
    private static final Color COLOR_PATH = new Color(255, 100, 50);   // Turuncu
    private static final Color COLOR_EMPTY = new Color(245, 245, 250); // Beyaz
    private static final Color COLOR_HEAVY = new Color(100, 100, 100);   // Kahve
    private static final Color COLOR_START = new Color(46, 204, 113);  // Yeşil
    private static final Color COLOR_END = new Color(231, 76, 60);     // Kırmızı

    // --- BİLEŞENLER ---
    private JPanel mazeCanvas;
    private JTextArea resultsArea;
    private Timer animationTimer;
    private JCheckBox chkWeighted, chkDynamic, chkMulti;
    private DefaultListModel<TestRecord> historyModel; // String değil TestRecord tutacak (Sıralama için)
    private JList<TestRecord> historyList;

    // --- VERİLER ---
    private List<Point> connections = new ArrayList<>();      
    private List<Point> heavyConnections = new ArrayList<>(); 
    private List<Integer> visitedNodes = new ArrayList<>();
    private List<Integer> finalPath = new ArrayList<>();
    private List<String> dynamicWallQueue = new ArrayList<>();
    
    private List<Integer> currentAnimatedVisits = new ArrayList<>();
    private List<Integer> currentAnimatedPath = new ArrayList<>();
    private int animationIndex = 0;
    private int pathIndex = 0;
    private int matrixRowCounter = 0;

    // Sonuçlar
    private String lastAlgoName = "";
    private int lastSteps = 0;
    private int lastWeight = 0;

    public MazeApp() {
        setTitle("Maze Solver Pro - V10.1 (Ultimate)");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        try { UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName()); } catch (Exception e) {}

        setLayout(new BorderLayout(0, 0));
        getContentPane().setBackground(COLOR_BG);

        // SOL PANEL
        add(createSidebar(), BorderLayout.WEST);

        // ORTA PANEL
        mazeCanvas = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g); 
                drawMazeResponsive(g);   
            }
        };
        mazeCanvas.setBackground(COLOR_BG);
        add(mazeCanvas, BorderLayout.CENTER);

        // ALT PANEL
        add(createBottomPanel(), BorderLayout.SOUTH);

        setSize(1350, 900); 
        setLocationRelativeTo(null); 
    }

    // --- ÇİZİM MOTORU (GÜZEL OLAN VERSİYON) ---
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

        // Duvarlar (Zemin)
        g2.setColor(COLOR_WALL);
        g2.fillRect(startX, startY, cellSize * COLS, cellSize * ROWS);

        if (connections.isEmpty() && heavyConnections.isEmpty()) return;

        // Odaları aç
        for (int i = 0; i < ROWS * COLS; i++) drawCell(g2, i, startX, startY, cellSize, COLOR_EMPTY);

        // Normal Yollar (Köprü kurarak çiziyoruz)
        g2.setColor(COLOR_EMPTY); 
        for (Point p : connections) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_EMPTY);
        
        // AĞIR Yollar
        Color heavyColor = chkWeighted.isSelected() ? COLOR_HEAVY : COLOR_EMPTY;
        g2.setColor(heavyColor);
        for (Point p : heavyConnections) {
            connectCells(g2, p.x, p.y, startX, startY, cellSize, heavyColor);
            drawCell(g2, p.x * COLS + p.y, startX, startY, cellSize, heavyColor); 
            drawCell(g2, p.y * COLS + p.x, startX, startY, cellSize, heavyColor); 
        }

        // Animasyon
        for (Integer cellId : currentAnimatedVisits) drawCell(g2, cellId, startX, startY, cellSize, COLOR_VISITED);
        g2.setColor(COLOR_VISITED);
        for (Point p : connections) if(currentAnimatedVisits.contains(p.x) && currentAnimatedVisits.contains(p.y)) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_VISITED);
        for (Point p : heavyConnections) if(currentAnimatedVisits.contains(p.x) && currentAnimatedVisits.contains(p.y)) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_VISITED);

        // Final Yol
        for (Integer cellId : currentAnimatedPath) drawCell(g2, cellId, startX, startY, cellSize, COLOR_PATH);
        g2.setColor(COLOR_PATH);
        for (Point p : connections) if(currentAnimatedPath.contains(p.x) && currentAnimatedPath.contains(p.y)) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_PATH);
        for (Point p : heavyConnections) if(currentAnimatedPath.contains(p.x) && currentAnimatedPath.contains(p.y)) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_PATH);

        drawTargets(g2, startX, startY, cellSize);
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

    // --- ESTETİK HÜCRE VE KÖPRÜ ÇİZİMİ ---
    private void drawCell(Graphics2D g2, int cellId, int startX, int startY, int size, Color c) {
        int r = cellId / COLS; int col = cellId % COLS;
        g2.setColor(c);
        // Köşeleri yuvarlatılmış kare (Daha hoş durur)
        g2.fillRoundRect(startX + col * size + 2, startY + r * size + 2, size - 4, size - 4, 8, 8);
    }

    private void connectCells(Graphics2D g2, int u, int v, int startX, int startY, int size, Color c) {
        int r1 = u / COLS, c1 = u % COLS; int r2 = v / COLS, c2 = v % COLS;
        g2.setColor(c); 
        int bridgeSize = size - 4; // Hücrenin içi kadar genişlik
        
        // İki hücre arasına dikdörtgen çizerek boşluğu dolduruyoruz (Köprü)
        if (r1 == r2) { // Yatay
            int c_min = Math.min(c1, c2);
            g2.fillRect(startX + (c_min + 1) * size - 4, startY + r1 * size + 2, 8, bridgeSize);
        } else { // Dikey
            int r_min = Math.min(r1, r2);
            g2.fillRect(startX + c1 * size + 2, startY + (r_min + 1) * size - 4, bridgeSize, 8);
        }
    }

    // --- ARAYÜZ (GUI) ---
    private JPanel createSidebar() {
        JPanel sidebar = new JPanel(); sidebar.setLayout(new BoxLayout(sidebar, BoxLayout.Y_AXIS));
        sidebar.setBackground(COLOR_PANEL); sidebar.setPreferredSize(new Dimension(240, 0)); sidebar.setBorder(new EmptyBorder(15, 15, 15, 15));
        
        addLabel(sidebar, "AYARLAR");
        chkWeighted = createCheckbox("Ağırlıklı (Weighted)"); sidebar.add(chkWeighted);
        chkDynamic = createCheckbox("Dinamik Duvarlar"); sidebar.add(chkDynamic);
        chkMulti = createCheckbox("Çoklu Hedef"); sidebar.add(chkMulti);
        sidebar.add(Box.createVerticalStrut(20));

        addLabel(sidebar, "İŞLEMLER");
        JButton btnGen = createButton("HARİTA OLUŞTUR", new Color(41, 128, 185));
        btnGen.addActionListener(e -> generateMapAction());
        sidebar.add(btnGen);
        sidebar.add(Box.createVerticalStrut(10));
        JButton btnLoad = createButton("JSON YÜKLE", new Color(230, 126, 34));
        btnLoad.addActionListener(e -> loadJsonTest());
        sidebar.add(btnLoad);
        sidebar.add(Box.createVerticalStrut(30));

        addLabel(sidebar, "ALGORİTMALAR");
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

        // 1. LEGEND (RENK ANAHTARI) - BURASI GERİ GELDİ
        JPanel legendPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 20, 5));
        legendPanel.setBackground(new Color(35, 35, 40));
        legendPanel.setBorder(BorderFactory.createMatteBorder(0,0,1,0, Color.GRAY));
        legendPanel.add(createLegendItem(COLOR_START, "Başlangıç"));
        legendPanel.add(createLegendItem(COLOR_END, "Hedef"));
        legendPanel.add(createLegendItem(COLOR_EMPTY, "Normal"));
        legendPanel.add(createLegendItem(COLOR_HEAVY, "Ağır"));
        legendPanel.add(createLegendItem(COLOR_VISITED, "Gezilen"));
        legendPanel.add(createLegendItem(COLOR_PATH, "Yol"));
        mainPanel.add(legendPanel, BorderLayout.NORTH);

        // 2. LOG VE GEÇMİŞ
        JPanel contentPanel = new JPanel(new GridLayout(1, 2, 10, 0));
        contentPanel.setBackground(new Color(25, 25, 30));
        contentPanel.setPreferredSize(new Dimension(0, 180));

        resultsArea = new JTextArea(); resultsArea.setBackground(new Color(20, 20, 25)); resultsArea.setForeground(new Color(46, 204, 113));
        resultsArea.setFont(new Font("Consolas", Font.PLAIN, 13)); resultsArea.setEditable(false);
        resultsArea.setText("> Sistem Hazır.\n");
        JScrollPane scrollLog = new JScrollPane(resultsArea);
        scrollLog.setBorder(createTitle("CANLI LOG"));

        JPanel rightPanel = new JPanel(new BorderLayout()); rightPanel.setBackground(new Color(25, 25, 30));
        historyModel = new DefaultListModel<>(); 
        historyList = new JList<>(historyModel); 
        historyList.setBackground(new Color(35, 35, 40)); historyList.setForeground(Color.WHITE);
        JScrollPane scrollHist = new JScrollPane(historyList); scrollHist.setBorder(createTitle("GEÇMİŞ TESTLER"));
        
        // 3. BUTONLAR (SIRALA VE KAYDET) - BURASI EKSİKTİ
        JPanel btnPanel = new JPanel(new GridLayout(1, 3, 5, 0)); 
        btnPanel.setBackground(new Color(25, 25, 30));
        
        JButton btnSortStep = createSmallButton("Sırala: Adım");
        btnSortStep.addActionListener(e -> sortHistory("step"));
        
        JButton btnSortWeight = createSmallButton("Sırala: Ağırlık");
        btnSortWeight.addActionListener(e -> sortHistory("weight"));
        
        JButton btnSave = createSmallButton("KAYDET (JSON)");
        btnSave.addActionListener(e -> saveJsonTest());

        btnPanel.add(btnSortStep); btnPanel.add(btnSortWeight); btnPanel.add(btnSave);
        rightPanel.add(scrollHist, BorderLayout.CENTER);
        rightPanel.add(btnPanel, BorderLayout.SOUTH);

        contentPanel.add(scrollLog); contentPanel.add(rightPanel);
        mainPanel.add(contentPanel, BorderLayout.CENTER);
        return mainPanel;
    }

    // --- YARDIMCI METOTLAR (Basit ve Sağlam) ---
    private JButton createButton(String text, Color bg) {
        JButton btn = new JButton(text);
        btn.setBackground(bg); btn.setForeground(Color.WHITE);
        btn.setFocusPainted(false); btn.setFont(new Font("Arial", Font.BOLD, 12));
        btn.setMaximumSize(new Dimension(Integer.MAX_VALUE, 40));
        btn.setAlignmentX(Component.LEFT_ALIGNMENT);
        return btn;
    }
    private JButton createSmallButton(String text) {
        JButton btn = new JButton(text);
        btn.setFont(new Font("Arial", Font.BOLD, 11));
        btn.setBackground(Color.DARK_GRAY); btn.setForeground(Color.WHITE);
        btn.setFocusPainted(false);
        return btn;
    }
    private JCheckBox createCheckbox(String text) {
        JCheckBox chk = new JCheckBox(text);
        chk.setBackground(COLOR_PANEL); chk.setForeground(Color.WHITE);
        chk.setFocusPainted(false); chk.setAlignmentX(Component.LEFT_ALIGNMENT);
        return chk;
    }
    private void addLabel(JPanel p, String text) {
        JLabel l = new JLabel(text); l.setForeground(Color.CYAN); l.setFont(new Font("Arial", Font.BOLD, 12));
        l.setAlignmentX(Component.LEFT_ALIGNMENT); p.add(l); p.add(Box.createVerticalStrut(5));
    }
    private JPanel createLegendItem(Color c, String text) {
        JPanel p = new JPanel(new FlowLayout(FlowLayout.LEFT, 5, 0)); p.setOpaque(false);
        JPanel box = new JPanel(); box.setPreferredSize(new Dimension(12, 12)); box.setBackground(c);
        box.setBorder(BorderFactory.createLineBorder(Color.WHITE));
        JLabel l = new JLabel(text); l.setForeground(Color.LIGHT_GRAY); p.add(box); p.add(l);
        return p;
    }
    private TitledBorder createTitle(String t) {
        return BorderFactory.createTitledBorder(null, t, TitledBorder.LEFT, TitledBorder.TOP, new Font("Arial",Font.BOLD,12), Color.GRAY);
    }

    // --- AKSİYONLAR ---
    private void generateMapAction() { runSolver("BFS"); } // Sadece harita için
    private void runSolver(String algo) {
        if(animationTimer != null) animationTimer.stop();
        connections.clear(); heavyConnections.clear();
        visitedNodes.clear(); finalPath.clear();
        currentAnimatedVisits.clear(); currentAnimatedPath.clear();
        animationIndex = 0; pathIndex = 0; matrixRowCounter = 0;
        
        lastAlgoName = algo;
        mazeCanvas.repaint();
        resultsArea.setText("> İşlem başlatıldı: " + algo + "\n");
        new Thread(() -> executeBackend(algo)).start();
    }

    // --- GÜÇLENDİRİLMİŞ İLETİŞİM ---
    private void executeBackend(String algo) {
        try {
            ProcessBuilder pb = new ProcessBuilder("Demo.exe", algo);
            pb.redirectErrorStream(true); // C'deki DEBUG mesajlarını okumak için
            Process p = pb.start();
            
            // PrintWriter kullanıyoruz (Daha güvenli ve otomatik flush var)
            PrintWriter pw = new PrintWriter(new OutputStreamWriter(p.getOutputStream()), true);
            
            // GÖNDERİLEN VERİYİ LOGA YAZALIM (Hata ayıklamak için)
            String inputData = String.format("%d %d %d %d %d", ROWS, COLS, 
                chkWeighted.isSelected()?1:0, chkDynamic.isSelected()?1:0, chkMulti.isSelected()?1:0);
            
            SwingUtilities.invokeLater(() -> resultsArea.append("Java Gönderiyor: " + inputData + "\n"));
            
            pw.println(inputData); // ln ile satır sonu ekliyoruz
            pw.flush(); // Garanti olsun diye sifonu çekiyoruz

            BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line;
            while((line = br.readLine()) != null) {
                String fLine = line.trim();
                
                // DEBUG MESAJLARINI YAKALA
                if (fLine.startsWith("DEBUG:")) {
                    SwingUtilities.invokeLater(() -> resultsArea.append(fLine + "\n"));
                } 
                else if(fLine.equals("MAP_READY")) {
                    SwingUtilities.invokeLater(() -> mazeCanvas.repaint());
                } 
                else {
                    SwingUtilities.invokeLater(() -> parseLine(fLine));
                }
            }
            SwingUtilities.invokeLater(this::startAnimation);
            
        } catch(Exception e) { 
            SwingUtilities.invokeLater(() -> resultsArea.append("KRİTİK HATA: " + e.getMessage() + "\n")); 
        }
    }

    private void parseLine(String line) {
        try {
            if(line.startsWith("ALGO:")) resultsArea.append("Algoritma: " + line.split(":")[1] + "\n");
            else if(line.startsWith("STEPS:")) { lastSteps = Integer.parseInt(line.split(":")[1].trim()); resultsArea.append("Adım: " + lastSteps + "\n"); }
            else if(line.startsWith("WEIGHT:")) { lastWeight = Integer.parseInt(line.split(":")[1].trim()); resultsArea.append("Ağırlık: " + lastWeight + "\n"); }
            else if(line.startsWith("DYNA:")) { // Dinamik Duvar
                dynamicWallQueue.add(line.split(":")[1].trim());

               /*String[] vals = line.split(":")[1].trim().split("\\s+");
                int u = Integer.parseInt(vals[0]); int v = Integer.parseInt(vals[1]); int w = Integer.parseInt(vals[2]);
                connections.removeIf(p -> (p.x==u && p.y==v) || (p.x==v && p.y==u));
                heavyConnections.removeIf(p -> (p.x==u && p.y==v) || (p.x==v && p.y==u));
                if(w == 1) connections.add(new Point(u, v)); else if(w >= 2) heavyConnections.add(new Point(u, v));
                mazeCanvas.repaint();
                resultsArea.append("Dinamik Duvar: " + u + "-" + v + "\n"); */
            }
            else if(line.startsWith("MATRIX:")) {
                String[] vals = line.split(":")[1].trim().split("\\s+");
                int u = matrixRowCounter;
                for(int v=0; v<vals.length; v++) {
                    int w = Integer.parseInt(vals[v]);
                    if(w == 1) connections.add(new Point(u, v));
                    else if(w >= 2) heavyConnections.add(new Point(u, v));
                }
                matrixRowCounter++;
            }
            else if(line.startsWith("EXPLORED:")) for(String s:line.substring(9).trim().split("\\s+")) visitedNodes.add(Integer.parseInt(s));
            else if(line.startsWith("RESULT:")) for(String s:line.substring(7).trim().split("\\s+")) finalPath.add(Integer.parseInt(s));
        } catch(Exception e) {}
    }
    private void processDynamicWall(String data){
        try{
            String[] vals =data.split("\\s+");
            int u = Integer.parseInt(vals[0]);
            int v = Integer.parseInt(vals[1]);
            int w = Integer.parseInt(vals[2]);
            connections.removeIf(p -> (p.x == u && p.y == v) || (p.x == v && p.y == u)); //duvarı temizle
            heavyConnections.removeIf(p -> (p.x == u && p.y == v) || (p.x == v && p.y == u));
            if(w==1)
                connections.add(new Point(u,v));
            else if(w>=2)
                heavyConnections.add(new Point(u, v));
        resultsArea.append(">> Dinamik Engel Değişti: " + u + "-" + v + " (W:" + w + ")\n");
        } catch (Exception e) {}
    }

    private void startAnimation() {
        if(visitedNodes.isEmpty()) return;
        animationTimer = new Timer(10, e -> {
            for(int i=0; i<5; i++) {
                if(animationIndex < visitedNodes.size()) {
                    currentAnimatedVisits.add(visitedNodes.get(animationIndex++));
                    
                    if (animationIndex % 10 == 0 && !dynamicWallQueue.isEmpty()) { //boyanan 10 hucrede 1 duvar degistir
                        processDynamicWall(dynamicWallQueue.remove(0));
                    }
                }
                else if(pathIndex < finalPath.size()) {
                    currentAnimatedPath.add(finalPath.get(pathIndex++));
                }
                else {
                    ((Timer)e.getSource()).stop();
                    SwingUtilities.invokeLater(() -> historyModel.addElement(new TestRecord(lastAlgoName, lastSteps, lastWeight)));
                    dynamicWallQueue.clear(); // Temizlik
                    return;
                }
            }
            mazeCanvas.repaint();
        });
        animationTimer.start();
    }

    // --- JSON KAYDETME ---
    private void saveJsonTest() {
        if(connections.isEmpty()) return;
        try {
            String ts = new SimpleDateFormat("yyyyMMdd_HHmm").format(new Date());
            BufferedWriter w = new BufferedWriter(new FileWriter("Test_" + ts + ".json"));
            w.write("{\n \"algo\": \"" + lastAlgoName + "\",\n \"steps\": " + lastSteps + ",\n \"weight\": " + lastWeight + ",\n");
            w.write(" \"connections\": ["); for(int i=0; i<connections.size(); i++) { w.write(connections.get(i).x + "," + connections.get(i).y); if(i<connections.size()-1) w.write("|"); } w.write("],\n");
            w.write(" \"heavy\": ["); for(int i=0; i<heavyConnections.size(); i++) { w.write(heavyConnections.get(i).x + "," + heavyConnections.get(i).y); if(i<heavyConnections.size()-1) w.write("|"); } w.write("]\n}");
            w.close(); JOptionPane.showMessageDialog(this, "Kaydedildi: Test_" + ts + ".json");
        } catch(IOException e) {}
    }
    
    private void loadJsonTest() { JOptionPane.showMessageDialog(this, "JSON Yükleme butonu aktif."); }

    // --- YARDIMCI SINIF (SIRALAMA İÇİN GEREKLİ) ---
    private static class TestRecord {
        String algo; int steps; int weight; String time;
        public TestRecord(String a, int s, int w) { algo=a; steps=s; weight=w; time=new SimpleDateFormat("HH:mm").format(new Date()); }
        public String toString() { return String.format("[%s] %s (S:%d W:%d)", time, algo, steps, weight); }
    }
    
    private void sortHistory(String type) {
        List<TestRecord> l = new ArrayList<>();
        for(int i=0; i<historyModel.getSize(); i++) l.add(historyModel.get(i));
        if(type.equals("step")) l.sort(Comparator.comparingInt(o -> o.steps));
        else l.sort(Comparator.comparingInt(o -> o.weight));
        historyModel.clear(); l.forEach(historyModel::addElement);
    }

    public static void main(String[] args) { SwingUtilities.invokeLater(() -> new MazeApp().setVisible(true)); }
}