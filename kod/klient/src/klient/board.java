/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package klient;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.JPanel;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.util.Arrays;


/**
 *
 * @author Wojtek
 * klasa przechowująca stan jednej gry
 */
public class board extends JPanel implements Runnable{
    
    private Socket CS;
    private InputStream is;
    private OutputStream os;
    private char G;
    private char[][][] f;
    private char w;//znak specjalny. wartosci: 'p'-gracz P, 'q'-gracz Q, 'w'-Wygrałeś, 'l'-Przegrałeś
    private char A;//mozliwość ataku. warości: 't', 'f'
    private BufferedImage b;
    private BufferedImage c;
    private BufferedImage p;
    private BufferedImage pk;
    private BufferedImage q;
    private BufferedImage qk;
    private JLabel JL;
    private JFrame JF;
    private JLabel END;
    /**
     * 
     * Plansza gry,
     * jej pełny rozmiar to 800*800 czyli 8*8 pól gdzie każde ma 100*100.
  
     * @param P JPanel na którym ma zostać wyświetlona plansza gry.
     * @param CS soket
     * @throws IOException 
     */
    
    public board(JPanel P, Socket CS, JLabel JL, JFrame JF, JLabel END) throws IOException{
        P.add(this);
        this.setCS(CS);
        this.JL=JL;
        this.JF=JF;
        this.END=END;
        this.is = this.getCS().getInputStream();
        this.os = this.getCS().getOutputStream();
        this.setLocation(0, 0);
        this.setSize(800, 800);
        this.setVisible(true);
        f = new char[3][8][8];
        this.setB(ImageIO.read(new File("obrazy/board.jpg")));
        this.setP(ImageIO.read(new File("obrazy/red.png")));
        this.setPK(ImageIO.read(new File("obrazy/red_cross.png")));
        this.setQ(ImageIO.read(new File("obrazy/black.png")));
        this.setQK(ImageIO.read(new File("obrazy/black_cross.png")));
        this.setC(ImageIO.read(new File("obrazy/target.png")));
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e){
                try {
                    int x, y;
                    String Z;
                    x=e.getX()/100;
                    y=e.getY()/100;
                    Z=Integer.toString(y)+Integer.toString(x);
                    os.write(Z.getBytes());
                } catch (IOException ex) {
                    Logger.getLogger(board.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        });
        
        
    }

    @Override
	public void paintComponent(Graphics g) {
                int i, j;
		super.paintComponent(g);
                char s;
                super.paintComponent(g);
                g.drawImage(this.getB(), 0, 0, null);
                for(i=0;i<8;i++){
                    for(j=0;j<8;j++){
                        s=this.getF(0, i, j);
                        switch(s){
                            case 'p': g.drawImage(this.getP(), j*100, i*100, null);
                            break;
                            case 'q': g.drawImage(this.getQ(), j*100, i*100, null);
                            break;
                            case 'P': g.drawImage(this.getPK(), j*100, i*100, null);
                            break;
                            case 'Q': g.drawImage(this.getQK(), j*100, i*100, null);
                            break;
                        }
                        if(this.getW()==this.getG()){
                            if(this.getA()=='t'){
                                if(this.getF(2, i, j)=='s') g.drawImage(this.getC(), j*100, i*100, null);
                                if(this.getF(1, i, j)=='a') g.drawImage(this.getC(), j*100, i*100, null);
                            }else{
                                if((this.getF(0, i, j)==this.getG())||(this.getF(0, i, j)==(char)this.getG()-32)) g.drawImage(this.getC(), j*100, i*100, null);
                                if(this.getF(1, i, j)=='m') g.drawImage(this.getC(), j*100, i*100, null);
                            }
                        }
                    }
                }
	}
        
        
    @Override
    public void run() {
        try {
            byte[] buffer = new byte[200];
            this.is.read(buffer);
            load(buffer);
            Arrays.fill(buffer, (byte)'0');
            this.setG(this.getW());
            boolean play=true;
            this.repaint();
            while(play){
                this.is.read(buffer);
                load(buffer);
                Arrays.fill(buffer, (byte)'0');
                this.repaint();
                if(this.getG()==this.getW()) JL.setText("Your Trun");
                else JL.setText("Oponent Trun");
                if((this.getW()=='l')||(this.getW()=='w')) play=false;
            }
            JF.setSize(JF.getPreferredSize());
            JF.setLocationRelativeTo(null);
            JF.setVisible(true);
            if(this.getW()=='w') END.setText("YOU WIN");
            else END.setText("YOU LOSE");
            this.getCS().close();
            this.getParent().remove(this);
        } catch (IOException ex) {
            Logger.getLogger(board.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
      
    
    /**
     *
     * Wczytuje pierwsze 192 znaki do tablicy f[][][], a następne ustawia jako W i  A.
     * 
     * @param buff Tablica typu 'byte'.
     * 
     */
    public void load(byte[] buff){
        String CON = new String(buff);
        char[] C=CON.toCharArray();
        int i, j, z, l=0;
        for(z=0; z<3;z++){
            for(i=0; i<8;i++){
                for(j=0; j<8;j++){
                    f[z][i][j]=C[l];
                    l++;
                }
            }
        }
        this.setW(C[l]);
        this.setA(C[l+1]);
    }
    
    /**
     * @param x
     * @param y
     * @param z
     * @return the f
     */
    public char getF(int x, int y, int z) {
        return f[x][y][z];
    }

    /**
     * @param f the char to setSSS
     * @param x
     * @param y
     * @param z
     */
    public void setF(char f, int x, int y, int z) {
        this.f[x][y][z] = f;
    }

    /**
     * @return the p
     */
    public BufferedImage getP() {
        return p;
    }

    /**
     * @param p the p to set
     */
    private void setP(BufferedImage p) {
        this.p = p;
    }

    /**
     * @return the P
     */
    public BufferedImage getPK() {
        return pk;
    }

    /**
     * @param P the P to set
     */
    private void setPK(BufferedImage P) {
        this.pk = P;
    }

    /**
     * @return the q
     */
    public BufferedImage getQ() {
        return q;
    }

    /**
     * @param q the q to set
     */
    private void setQ(BufferedImage q) {
        this.q = q;
    }

    /**
     * @return the Q
     */
    public BufferedImage getQK() {
        return qk;
    }

    /**
     * @param Q the Q to set
     */
    private void setQK(BufferedImage Q) {
        this.qk = Q;
    }

    /**
     * @return the b
     */
    public BufferedImage getB() {
        return b;
    }

    /**
     * @param b the b to set
     */
    private void setB(BufferedImage b) {
        this.b = b;
    }

    /**
     * @return the c
     */
    public BufferedImage getC() {
        return c;
    }

    /**
     * @param c the c to set
     */
    private void setC(BufferedImage c) {
        this.c = c;
    }

    /**
     * @return the w
     */
    public char getW() {
        return w;
    }

    /**
     * @param w the w to set
     */
    public void setW(char w) {
        this.w = w;
    }

    /**
     * @return the A
     */
    public char getA() {
        return A;
    }

    /**
     * @param A the A to set
     */
    public void setA(char A) {
        this.A = A;
    }

    /**
     * @return the G
     */
    public char getG() {
        return G;
    }

    /**
     * @param G the G to set
     */
    public void setG(char G) {
        this.G = G;
    }

    /**
     * @return the CS
     */
    public Socket getCS() {
        return CS;
    }

    /**
     * @param CS the CS to set
     */
    public void setCS(Socket CS) {
        this.CS = CS;
    }
}
