import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class MazeApp extends JFrame {

    // --- AYARLAR ---
    private static final boolean USE_MOCK = true; // Sunumda FALSE yap!
    
    // Varsayılan Değerler
    private static int ROWS = 20; 
    private static int COLS = 20;

    // --- RENKLER ---
    private static final Color COLOR_BG = new Color(30, 30, 35); 
    private static final Color COLOR_PANEL = new Color(40, 44, 50);
    private static final Color COLOR_WALL = new Color(60, 60, 65);
    private static final Color COLOR_VISITED = new Color(0, 190, 240); 
    private static final Color COLOR_PATH = new Color(255, 100, 50);   
    private static final Color COLOR_EMPTY = new Color(245, 245, 250);

    // --- BİLEŞENLER ---
    private JPanel mazeCanvas;
    private JTextArea resultsArea;
    private Timer animationTimer;
    private JTextField txtRows, txtCols;
    
    // Checkboxları sınıf seviyesinde tanımladık ki okuyabilelim
    private JCheckBox chkWeighted, chkDynamic, chkMulti; 

    // --- VERİLER ---
    private List<Point> connections = new ArrayList<>();
    private List<Integer> visitedNodes = new ArrayList<>();
    private List<Integer> finalPath = new ArrayList<>();
    
    // Animasyon
    private List<Integer> currentAnimatedVisits = new ArrayList<>();
    private List<Integer> currentAnimatedPath = new ArrayList<>();
    private int animationIndex = 0;
    private int pathIndex = 0;

    public MazeApp() {
        setTitle("Maze Solver Pro - V3.2 (Full Connected)");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        try { UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName()); } catch (Exception e) {}

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

        setSize(1250, 850); 
        setLocationRelativeTo(null); 
        
        SwingUtilities.invokeLater(this::generateMapAction);
    }

    // --- RESPONSIVE ÇİZİM ---
    private void drawMazeResponsive(Graphics g) {
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        int panelW = mazeCanvas.getWidth();
        int panelH = mazeCanvas.getHeight();

        int availableW = panelW - 40; 
        int availableH = panelH - 40;

        int cellW = availableW / COLS;
        int cellH = availableH / ROWS;
        int cellSize = Math.min(cellW, cellH); 
        cellSize = Math.max(cellSize, 4); 

        int totalMazeW = cellSize * COLS;
        int totalMazeH = cellSize * ROWS;
        int startX = (panelW - totalMazeW) / 2;
        int startY = (panelH - totalMazeH) / 2;

        g2.setColor(COLOR_WALL);
        g2.fillRect(startX, startY, totalMazeW, totalMazeH);

        for (int i = 0; i < ROWS * COLS; i++) drawCell(g2, i, startX, startY, cellSize, COLOR_EMPTY);

        g2.setColor(COLOR_EMPTY); 
        for (Point p : connections) connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_EMPTY);

        for (Integer cellId : currentAnimatedVisits) drawCell(g2, cellId, startX, startY, cellSize, COLOR_VISITED);
        
        g2.setColor(COLOR_VISITED);
        for (Point p : connections) {
            if(currentAnimatedVisits.contains(p.x) && currentAnimatedVisits.contains(p.y)) 
                connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_VISITED);
        }

        for (Integer cellId : currentAnimatedPath) drawCell(g2, cellId, startX, startY, cellSize, COLOR_PATH);
        
        g2.setColor(COLOR_PATH);
        for (Point p : connections) {
            if(currentAnimatedPath.contains(p.x) && currentAnimatedPath.contains(p.y)) 
                connectCells(g2, p.x, p.y, startX, startY, cellSize, COLOR_PATH);
        }
    }

    private void drawCell(Graphics2D g2, int cellId, int startX, int startY, int size, Color c) {
        int r = cellId / COLS;
        int col = cellId % COLS;
        int x = startX + (col * size);
        int y = startY + (r * size);
        int padding = Math.max(1, size / 10); 
        int arc = Math.max(2, size / 4); 
        g2.setColor(c);
        g2.fillRoundRect(x + padding, y + padding, size - 2*padding, size - 2*padding, arc, arc);
    }

    private void connectCells(Graphics2D g2, int u, int v, int startX, int startY, int size, Color c) {
        int r1 = u / COLS, c1 = u % COLS;
        int r2 = v / COLS, c2 = v % COLS;
        int padding = Math.max(1, size / 10);
        int bridgeThick = size - 2*padding;
        g2.setColor(c);
        int x1 = startX + (c1 * size);
        int y1 = startY + (r1 * size);

        if (r1 == r2) { 
            int c_min = Math.min(c1, c2);
            int bridgeX = startX + ((c_min + 1) * size) - padding - (size/4); 
            g2.fillRect(bridgeX, y1 + padding, (padding*2) + (size/2), bridgeThick);
        } else { 
            int r_min = Math.min(r1, r2);
            int bridgeY = startY + ((r_min + 1) * size) - padding - (size/4);
            g2.fillRect(x1 + padding, bridgeY, bridgeThick, (padding*2) + (size/2));
        }
    }

    // --- ARAYÜZ ---
    private JPanel createSidebar() {
        JPanel sidebar = new JPanel();
        sidebar.setLayout(new BoxLayout(sidebar, BoxLayout.Y_AXIS));
        sidebar.setBackground(COLOR_PANEL); 
        sidebar.setPreferredSize(new Dimension(240, 0));
        sidebar.setBorder(new EmptyBorder(25, 20, 25, 20));

        addLabel(sidebar, "HARİTA BOYUTU");
        JPanel pnlInput = new JPanel(new GridLayout(1, 2, 10, 0));
        pnlInput.setBackground(COLOR_PANEL);
        pnlInput.setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
        pnlInput.setAlignmentX(Component.LEFT_ALIGNMENT);
        txtRows = createDarkTextField("" + ROWS);
        txtCols = createDarkTextField("" + COLS);
        pnlInput.add(txtRows); pnlInput.add(txtCols);
        sidebar.add(pnlInput);
        
        JLabel lblInfo = new JLabel(" (Önerilen: 20x20)");
        lblInfo.setForeground(Color.GRAY);
        lblInfo.setFont(new Font("SansSerif", Font.ITALIC, 11));
        sidebar.add(lblInfo);
        sidebar.add(Box.createVerticalStrut(20));

        addLabel(sidebar, "İŞLEMLER");
        JButton btnGen = createStyledButton("Harita Oluştur", new Color(41, 128, 185));
        btnGen.addActionListener(e -> generateMapAction());
        sidebar.add(btnGen);
        sidebar.add(Box.createVerticalStrut(10));
        
        JButton btnLoad = createStyledButton("Test Yükle (.txt)", new Color(127, 140, 141));
        btnLoad.addActionListener(e -> loadMapAction());
        sidebar.add(btnLoad);
        sidebar.add(Box.createVerticalStrut(30));

        addLabel(sidebar, "AYARLAR");
        // Checkboxları değişkene atıyoruz
        chkWeighted = createDarkCheckbox("Ağırlıklı (Weighted)");
        chkDynamic = createDarkCheckbox("Dinamik Duvarlar");
        chkMulti = createDarkCheckbox("Çoklu Hedef");
        
        sidebar.add(chkWeighted);
        sidebar.add(chkDynamic);
        sidebar.add(chkMulti);
        sidebar.add(Box.createVerticalStrut(30));

        addLabel(sidebar, "ALGORİTMALAR");
        String[] algos = {"BFS", "DFS", "Dijkstra", "A*"};
        for (String algo : algos) {
            JButton btn = createStyledButton(algo, new Color(39, 174, 96));
            btn.addActionListener(e -> runSolver(algo));
            sidebar.add(btn);
            sidebar.add(Box.createVerticalStrut(10));
        }
        return sidebar;
    }

    private JCheckBox createDarkCheckbox(String text) {
        JCheckBox chk = new JCheckBox(text);
        chk.setBackground(COLOR_PANEL);
        chk.setForeground(Color.LIGHT_GRAY);
        chk.setFocusPainted(false);
        chk.setFont(new Font("Segoe UI", Font.PLAIN, 13));
        chk.setAlignmentX(Component.LEFT_ALIGNMENT);
        return chk;
    }

    private void addLabel(JPanel panel, String text) {
        JLabel lbl = new JLabel(text);
        lbl.setForeground(new Color(100, 200, 250));
        lbl.setFont(new Font("SansSerif", Font.BOLD, 12));
        lbl.setAlignmentX(Component.LEFT_ALIGNMENT);
        panel.add(lbl);
        panel.add(Box.createVerticalStrut(10));
    }

    private JTextField createDarkTextField(String text) {
        JTextField txt = new JTextField(text);
        txt.setBackground(new Color(60, 63, 65));
        txt.setForeground(Color.WHITE);
        txt.setCaretColor(Color.WHITE);
        txt.setBorder(BorderFactory.createLineBorder(new Color(80, 80, 80)));
        txt.setHorizontalAlignment(JTextField.CENTER);
        return txt;
    }

    private JButton createStyledButton(String text, Color bg) {
        JButton btn = new JButton(text);
        btn.setAlignmentX(Component.LEFT_ALIGNMENT);
        btn.setMaximumSize(new Dimension(Integer.MAX_VALUE, 40));
        btn.setBackground(bg);
        btn.setForeground(Color.WHITE);
        btn.setFocusPainted(false);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 13));
        btn.setOpaque(true); btn.setBorderPainted(false); 
        return btn;
    }

    private JPanel createBottomPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(new Color(25, 25, 30));
        panel.setPreferredSize(new Dimension(0, 140));
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        resultsArea = new JTextArea();
        resultsArea.setBackground(new Color(20, 20, 25));
        resultsArea.setForeground(new Color(46, 204, 113));
        resultsArea.setFont(new Font("Consolas", Font.PLAIN, 14));
        resultsArea.setEditable(false);
        resultsArea.setText("> Sistem Hazır. V3.2 (Settings Enabled)\n");

        panel.add(new JScrollPane(resultsArea), BorderLayout.CENTER);
        return panel;
    }

    // --- AKSİYONLAR ---
    private void generateMapAction() {
        if (animationTimer != null) animationTimer.stop();
        connections.clear(); visitedNodes.clear(); finalPath.clear();
        currentAnimatedVisits.clear(); currentAnimatedPath.clear();
        animationIndex = 0; pathIndex = 0;
        mazeCanvas.repaint();

        try {
            int newR = Integer.parseInt(txtRows.getText().trim());
            int newC = Integer.parseInt(txtCols.getText().trim());
            if(newR < 2 || newC < 2) return;
            ROWS = newR; COLS = newC;
        } catch(Exception e) { return; }

        resultsArea.append("\n> Yeni Harita Oluşturuluyor (" + ROWS + "x" + COLS + ")...\n");
        for(int i=0; i<ROWS*COLS; i++) {
             if(i % COLS < COLS-1 && Math.random() > 0.45) connections.add(new Point(i, i+1));
             if(i / COLS < ROWS-1 && Math.random() > 0.45) connections.add(new Point(i, i+COLS));
        }
        mazeCanvas.repaint(); 
    }

    private void loadMapAction() {
        JFileChooser fc = new JFileChooser(new File("."));
        if(fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                BufferedReader br = new BufferedReader(new FileReader(fc.getSelectedFile()));
                connections.clear(); visitedNodes.clear(); finalPath.clear();
                currentAnimatedVisits.clear(); currentAnimatedPath.clear();
                if(animationTimer!=null) animationTimer.stop();

                String line = br.readLine();
                if(line!=null) ROWS = Integer.parseInt(line.trim());
                line = br.readLine();
                if(line!=null) COLS = Integer.parseInt(line.trim());
                txtRows.setText(""+ROWS); txtCols.setText(""+COLS);

                while((line = br.readLine()) != null) {
                    String[] p = line.trim().split("\\s+");
                    if(p.length >= 2) connections.add(new Point(Integer.parseInt(p[0]), Integer.parseInt(p[1])));
                }
                br.close();
                mazeCanvas.repaint();
                resultsArea.append("> Harita Yüklendi.\n");
            } catch(Exception e) {}
        }
    }

    private void runSolver(String algo) {
        visitedNodes.clear(); finalPath.clear(); 
        currentAnimatedVisits.clear(); currentAnimatedPath.clear();
        animationIndex = 0; pathIndex = 0;
        if(animationTimer != null) animationTimer.stop();
        mazeCanvas.repaint();

        resultsArea.append("> Algoritma: " + algo + " başlatılıyor...\n");
        new Thread(() -> {
            if(USE_MOCK) simulateBackend(algo);
            else executeBackend(algo);
        }).start();
    }

    private void startAnimation() {
        if(visitedNodes.isEmpty()) return;
        animationTimer = new Timer(20, new ActionListener() { 
            @Override
            public void actionPerformed(ActionEvent e) {
                int speedExploration = 5; int speedPath = 1;
                boolean isExploring = (animationIndex < visitedNodes.size());
                int currentSpeed = isExploring ? speedExploration : speedPath;
                
                for(int i = 0; i < currentSpeed; i++) {
                    if(animationIndex < visitedNodes.size()) currentAnimatedVisits.add(visitedNodes.get(animationIndex++));
                    else if (pathIndex < finalPath.size()) {
                        if (isExploring) break; 
                        currentAnimatedPath.add(finalPath.get(pathIndex++));
                    } else {
                        mazeCanvas.repaint(); ((Timer)e.getSource()).stop();
                        resultsArea.append("> Bitti.\n"); return;
                    }
                }
                mazeCanvas.repaint(); 
            }
        });
        animationTimer.start();
    }

    // --- BURASI KRİTİK: BACKEND İLETİŞİMİ ---
    private void simulateBackend(String algo) {
        try {
            Thread.sleep(200);
            parseLine("ALGO: " + algo);
            parseLine("STEPS: 100");
            
            // Ayarların seçilip seçilmediğini loga yazalım ki çalıştığını gör
            if(chkWeighted.isSelected()) resultsArea.append("[INFO] Ağırlıklı mod aktif.\n");
            if(chkDynamic.isSelected()) resultsArea.append("[INFO] Dinamik duvar aktif.\n");

            StringBuilder expl = new StringBuilder("EXPLORED:");
            for(int i=0; i<ROWS*COLS/2; i++) expl.append(" ").append(i);
            parseLine(expl.toString());

            StringBuilder res = new StringBuilder("RESULT:");
            int curr=0;
            while(curr<ROWS*COLS-1) {
                res.append(" ").append(curr);
                if(Math.random()>0.5 && (curr+1)%COLS!=0) curr++; else if(curr+COLS < ROWS*COLS) curr+=COLS; else break;
            }
            parseLine(res.toString());
            SwingUtilities.invokeLater(this::startAnimation);
        } catch(Exception e) {}
    }

    private void executeBackend(String algo) {
        try {
            ProcessBuilder pb = new ProcessBuilder("Demo.exe", algo);
            pb.redirectErrorStream(true);
            Process p = pb.start();
            
            BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(p.getOutputStream()));
            
            // --- GÜNCELLEME BURADA ---
            // Sadece ROWS ve COLS değil, Checkbox durumlarını da gönderiyoruz!
            // Format: ROWS [enter] COLS [enter] WEIGHTED [enter] DYNAMIC [enter] MULTI [enter]
            
            bw.write(ROWS + "\n");
            bw.write(COLS + "\n");
            bw.write((chkWeighted.isSelected() ? "1" : "0") + "\n");
            bw.write((chkDynamic.isSelected() ? "1" : "0") + "\n");
            bw.write((chkMulti.isSelected() ? "1" : "0") + "\n");
            bw.flush();
            // ------------------------
            
            BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line;
            while((line = br.readLine()) != null) {
                String fLine = line.trim();
                SwingUtilities.invokeLater(() -> parseLine(fLine));
            }
            SwingUtilities.invokeLater(this::startAnimation);
        } catch(Exception e) { SwingUtilities.invokeLater(() -> resultsArea.append("Hata: " + e.getMessage() + "\n")); }
    }

    private void parseLine(String line) {
        try {
            if(line.startsWith("ALGO:")) resultsArea.append("Algo: " + line.split(":")[1] + "\n");
            else if(line.startsWith("EXPLORED:")) {
                String[] p = line.substring(9).trim().split("\\s+");
                for(String s:p) visitedNodes.add(Integer.parseInt(s));
            } else if(line.startsWith("RESULT:")) {
                String[] p = line.substring(7).trim().split("\\s+");
                for(String s:p) finalPath.add(Integer.parseInt(s));
            }
        } catch(Exception e) {}
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new MazeApp().setVisible(true));
    }
}