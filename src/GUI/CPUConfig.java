/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package GUI;

import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import FileManager.*;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.concurrent.TimeUnit;

/**
 *
 * @author cesar
 */
public class CPUConfig extends javax.swing.JFrame {

    /**
     * Creates new form CPUConfig
     */
    private String _actualPath;
    
    public CPUConfig() {
        this._actualPath = "";
        initComponents();
        
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();
        jScrollPane5 = new javax.swing.JScrollPane();
        editorTextPane = new javax.swing.JTextPane();
        jMenuBar1 = new javax.swing.JMenuBar();
        jMenu1 = new javax.swing.JMenu();
        jMenuItem2 = new javax.swing.JMenuItem();
        jMenuItem1 = new javax.swing.JMenuItem();
        jMenu2 = new javax.swing.JMenu();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setMaximumSize(new java.awt.Dimension(600, 700));
        setMinimumSize(new java.awt.Dimension(600, 700));
        setPreferredSize(new java.awt.Dimension(600, 700));

        jButton1.setText("Cancelar");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jButton2.setText("Compilar");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton2ActionPerformed(evt);
            }
        });

        editorTextPane.setSize(769, 422);
        editorTextPane.setEditorKit(new javax.swing.text.StyledEditorKit());
        editorTextPane.setCursor(new java.awt.Cursor(java.awt.Cursor.TEXT_CURSOR));
        editorTextPane.setDisabledTextColor(new java.awt.Color(204, 204, 255));
        editorTextPane.setSelectionColor(new java.awt.Color(153, 153, 153));
        jScrollPane5.setViewportView(editorTextPane);
        TextLineNumber tln = new TextLineNumber( editorTextPane );
        TextWordsColor twc = new TextWordsColor(editorTextPane);
        LinePainter painter = new LinePainter(editorTextPane);
        jScrollPane5.setRowHeaderView( tln );

        jMenu1.setText("File");

        jMenuItem2.setText("Abrir");
        jMenuItem2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem2ActionPerformed(evt);
            }
        });
        jMenu1.add(jMenuItem2);

        jMenuItem1.setText("Guardar");
        jMenuItem1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem1ActionPerformed(evt);
            }
        });
        jMenu1.add(jMenuItem1);

        jMenuBar1.add(jMenu1);

        jMenu2.setText("Edit");
        jMenuBar1.add(jMenu2);

        setJMenuBar(jMenuBar1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addGap(0, 568, Short.MAX_VALUE)
                        .addComponent(jButton1)
                        .addGap(18, 18, 18)
                        .addComponent(jButton2))
                    .addComponent(jScrollPane5, javax.swing.GroupLayout.Alignment.TRAILING))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(23, 23, 23)
                .addComponent(jScrollPane5, javax.swing.GroupLayout.PREFERRED_SIZE, 582, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 19, Short.MAX_VALUE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton2)
                    .addComponent(jButton1))
                .addGap(28, 28, 28))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton2ActionPerformed
        // TODO add your handling code here:
        try {
            String cmd = "./scriptV.sh"; //Comando de apagado en linux
            Runtime.getRuntime().exec(cmd);
            TimeUnit.SECONDS.sleep(1);
            determinaTamano();
        } catch (IOException ioe) {
            System.out.println (ioe);
        } catch (InterruptedException ex) {
            Logger.getLogger(CPUConfig.class.getName()).log(Level.SEVERE, null, ex);
        }    
    }//GEN-LAST:event_jButton2ActionPerformed

    
    
    public static void determinaTamano() throws FileNotFoundException, IOException{
        String cadena;
        String archivo = "Programa.dump";
        String lineaInicial = "";
        String lineaFinal = "";
        boolean flagInicial = false;
        FileReader f = new FileReader(archivo);
        BufferedReader b = new BufferedReader(f);
        while((cadena = b.readLine())!=null) {
            if (cadena.contains(">") && !flagInicial){
                lineaInicial = b.readLine();
                flagInicial = true;
            }
            lineaFinal = cadena;
        }
        b.close();
        
        System.out.println(lineaInicial);
        System.out.println(lineaFinal);
        
        int valorInicial = Integer.parseInt(lineaInicial.substring(3, lineaInicial.indexOf(":")), 16);
        System.out.println(valorInicial);
        
        int valorFinal = Integer.parseInt(lineaFinal.substring(3, lineaFinal.indexOf(":")), 16);
        System.out.println(valorFinal);
        
        int tamano = (valorFinal - valorInicial)/2;
        
        int tamanoFinal = 0;
        int potencia = 2;
        
        while (Math.pow(2,potencia) < tamano){
            potencia += 1;
        }
        
        tamanoFinal = (int) Math.pow(2, potencia);
        
        System.out.println(tamanoFinal);
        
        analizaGenerado(tamanoFinal);
        
    }
    
    public static void analizaGenerado (int tamano) throws FileNotFoundException, IOException{
        String cadena;
        FileReader f = new FileReader("Programa.txt");
        BufferedReader b = new BufferedReader(f);
        while((cadena = b.readLine()).contains("00000000")) {
            
        }
        
        FileWriter fichero = null;
        PrintWriter pw = null;
        try
        {
            fichero = new FileWriter("CodigoPrograma.txt");
            pw = new PrintWriter(fichero);
            pw.println(cadena);
            for (int i = 0; i < tamano; i++){
                cadena = b.readLine();
                if (cadena == "00000000")
                    pw.println("00000000");  
                else{
                    pw.println(cadena);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
           try {
           // Nuevamente aprovechamos el finally para 
           // asegurarnos que se cierra el fichero.
           if (null != fichero)
              fichero.close();
           } catch (Exception e2) {
              e2.printStackTrace();
           }
        }
        
        
        b.close();
    }
    
    
    
    private void jMenuItem1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem1ActionPerformed
        // TODO add your handling code here:
        String textoAGuardar;
        if(!this._actualPath.isEmpty())
        {
           // El texto NO está vacío y se ha abierto o guardado como... 
            int respuesta = JOptionPane.showConfirmDialog(null, "¿Desea actualizar "
                + "los cambios?\n"
                + "(Cambios que no son guardados se borrarán)", 
                "Guardar", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
            
            if (JOptionPane.YES_OPTION == respuesta){
                textoAGuardar = editorTextPane.getText();
                guardarSinFileChooser(textoAGuardar);    
            }
        }else{
            textoAGuardar = editorTextPane.getText();
            guardarComoFileChooser(textoAGuardar);
        }
    }//GEN-LAST:event_jMenuItem1ActionPerformed

      private void cargarFileChooser()
    {
        JFrame parentFrame = new JFrame();
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setDialogTitle("Abrir...");
        String texto = "";
        int userSelection = fileChooser.showOpenDialog(parentFrame);
        if (userSelection == JFileChooser.APPROVE_OPTION) {
            File fileToLoad = fileChooser.getSelectedFile();
            try {
                if(fileToLoad.getAbsolutePath().endsWith(".c")){
                    this._actualPath = null;
                    editorTextPane.setText("");
                    texto = File_TXT.readText(fileToLoad.getAbsolutePath());
                    this._actualPath = fileToLoad.getAbsolutePath();
                    editorTextPane.setText(texto);
                }else{
                    JOptionPane.showMessageDialog(this, "¡Formato de archivo"
                  + " no es compatible!",
                "Cargar", JOptionPane.ERROR_MESSAGE);
                }
            } catch (IOException ex) {
                Logger.getLogger(CPUConfig.class.getName()).log(Level.SEVERE, null, ex);
            }    
        }
        
    }
    
    
    
    private void jMenuItem2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem2ActionPerformed
        // TODO add your handling code here:
        cargarFileChooser();
    }//GEN-LAST:event_jMenuItem2ActionPerformed

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        // TODO add your handling code here:
        dispose();
    }//GEN-LAST:event_jButton1ActionPerformed

    
    
    private void guardarSinFileChooser(String textoAGuardar)
    {
        try {
            File_TXT.writeText(this._actualPath, textoAGuardar);
        } catch (IOException ex) {
            Logger.getLogger(CPUConfig.class.getName()).log(Level.SEVERE, null, ex);
        }
            JOptionPane.showMessageDialog(this, "¡Archivo actualizado y"
                    + " guardado!", "Confirmación", JOptionPane.INFORMATION_MESSAGE);
    }
    
    
    
    
     private void guardarComoFileChooser(String textoAGuardar)
    {
        JFrame parentFrame = new JFrame();
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setDialogTitle("Guardar como...");
        int userSelection = fileChooser.showSaveDialog(parentFrame);
        if (userSelection == JFileChooser.APPROVE_OPTION) {
            File fileToSave = fileChooser.getSelectedFile();
            if (fileToSave.getAbsolutePath().endsWith(".isi") &&
                !File_TXT.verifyFilenameExists(fileToSave.getAbsolutePath())) {
                        this._actualPath = fileToSave.getAbsolutePath();
                try {
                    File_TXT.writeText(fileToSave.getAbsolutePath(), textoAGuardar);
                } catch (IOException ex) {
                    Logger.getLogger(CPUConfig.class.getName()).log(Level.SEVERE, null, ex);
                }
                    JOptionPane.showMessageDialog(this, "¡Archivo guardado"
                  + " con éxito!", "Confirmación", JOptionPane.INFORMATION_MESSAGE);
                        
            }else if (!fileToSave.getAbsolutePath().endsWith(".isi") &&
                !File_TXT.verifyFilenameExists(fileToSave.getAbsolutePath() + ".isi")){
                this._actualPath = fileToSave.getAbsolutePath() + ".isi";
                try {
                    File_TXT.writeText(fileToSave.getAbsolutePath(), textoAGuardar);
                } catch (IOException ex) {
                    Logger.getLogger(CPUConfig.class.getName()).log(Level.SEVERE, null, ex);
                }
                    JOptionPane.showMessageDialog(this, "¡Archivo guardado"
                  + " con éxito!", "Confirmación", JOptionPane.INFORMATION_MESSAGE);
            } else{
                 JOptionPane.showMessageDialog(this, "¡Ya existe archivo "
                  + "con ese nombre!",
                "Guardar como", JOptionPane.WARNING_MESSAGE);
            }
        }
    }
     
     
    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
         */
        try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(CPUConfig.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(CPUConfig.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(CPUConfig.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(CPUConfig.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new CPUConfig().setVisible(true);
            }
        });
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextPane editorTextPane;
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton2;
    private javax.swing.JMenu jMenu1;
    private javax.swing.JMenu jMenu2;
    private javax.swing.JMenuBar jMenuBar1;
    private javax.swing.JMenuItem jMenuItem1;
    private javax.swing.JMenuItem jMenuItem2;
    private javax.swing.JScrollPane jScrollPane5;
    // End of variables declaration//GEN-END:variables
}
