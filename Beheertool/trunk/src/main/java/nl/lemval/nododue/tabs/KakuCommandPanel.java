/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * KakuCommandPanel.java
 *
 * Created on 21-mrt-2010, 20:24:49
 */

package nl.lemval.nododue.tabs;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.ListCellRenderer;
import javax.swing.SwingConstants;
import nl.lemval.nododue.NodoDueManagerView;
import nl.lemval.nododue.Options;
import nl.lemval.nododue.component.KakuButton;
import nl.lemval.nododue.util.listeners.KakuEventListener;
import org.jdesktop.layout.GroupLayout;

/**
 *
 * @author Michael
 */
public class KakuCommandPanel extends NodoBasePanel {

    private ListCellRenderer centeredCellRenderer;

        /** Creates new form KakuCommandPanel */
    public KakuCommandPanel(NodoDueManagerView view) {
        super(view);

        centeredCellRenderer = new ListCellRenderer() {
            private ListCellRenderer defaultRenderer = new DefaultListCellRenderer();
            public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
                Component comp = defaultRenderer.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
                ((DefaultListCellRenderer) comp).setHorizontalAlignment(SwingConstants.CENTER);
                return comp;
            }
        };

        initComponents();
        buildKakuPanel();

        String dc = Options.getInstance().getDomesticCode();
        channelList.setSelectedValue(dc, true);
    }

    private void buildKakuPanel() {
        buttonPanel.setLayout(new GridLayout(2,2));
        JPanel group1 = new javax.swing.JPanel();
        JPanel group2 = new javax.swing.JPanel();
        JPanel group3 = new javax.swing.JPanel();
        JPanel group4 = new javax.swing.JPanel();
        group1.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        group2.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        group3.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        group4.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        buttonPanel.add(group1);
        buttonPanel.add(group3);
        buttonPanel.add(group2);
        buttonPanel.add(group4);

        GroupLayout[] gl = new GroupLayout[4];
        gl[0] = new GroupLayout(group1); group1.setLayout(gl[0]);
        gl[1] = new GroupLayout(group2); group2.setLayout(gl[1]);
        gl[2] = new GroupLayout(group3); group3.setLayout(gl[2]);
        gl[3] = new GroupLayout(group4); group4.setLayout(gl[3]);

        KakuEventListener listener = new KakuEventListener(channelList, getListener());
        Dimension dimension = new Dimension(73, 23);

        GroupLayout.SequentialGroup[] sg = new GroupLayout.SequentialGroup[4];
        GroupLayout.ParallelGroup[] pg = new GroupLayout.ParallelGroup[4];

        int count = 1;

        for (int i = 0; i < gl.length; i++) {
            GroupLayout groupLayout = gl[i];
            for (int j = 0; j < 4; j++) {
                JButton on = new KakuButton(count, true);
                JButton off = new KakuButton(count, false);
                on.addActionListener(listener);
                off.addActionListener(listener);
                on.setMinimumSize(dimension);
                off.setMinimumSize(dimension);
                JLabel label = new JLabel(String.valueOf(count), JLabel.CENTER);
//                label.setFont(getResourceFont("kakuButton.font"));
                sg[j] = groupLayout.createSequentialGroup()
                            .add(on)
                            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                            .add(label, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 41, Short.MAX_VALUE)
                            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                            .add(off);
                pg[j] = groupLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                        .add(on)
                        .add(off)
                        .add(label);
                count++;
            }

            groupLayout.setHorizontalGroup(
                groupLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                .add(groupLayout.createSequentialGroup()
                    .addContainerGap()
                    .add(groupLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                        .add(sg[0])
                        .add(sg[1])
                        .add(sg[2])
                        .add(sg[3]))
                    .addContainerGap())
            );
            groupLayout.setVerticalGroup(
                groupLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                .add(groupLayout.createSequentialGroup()
                    .addContainerGap()
                    .add(pg[0])
                    .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                    .add(pg[1])
                    .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                    .add(pg[2])
                    .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                    .add(pg[3])
                    .addContainerGap())//139, Short.MAX_VALUE))
            );
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        channelLabel = new javax.swing.JLabel();
        channelList = new javax.swing.JList();
        buttonPanel = new javax.swing.JPanel();

        setName("Form"); // NOI18N

        channelLabel.setFont(channelLabel.getFont().deriveFont(channelLabel.getFont().getStyle() | java.awt.Font.BOLD, channelLabel.getFont().getSize()+1));
        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class).getContext().getResourceMap(KakuCommandPanel.class);
        channelLabel.setText(resourceMap.getString("channelLabel.text")); // NOI18N
        channelLabel.setName("channelLabel"); // NOI18N

        channelList.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        channelList.setFont(channelList.getFont().deriveFont(channelList.getFont().getStyle() | java.awt.Font.BOLD, channelList.getFont().getSize()+1));
        channelList.setModel(new javax.swing.AbstractListModel() {
            String[] strings = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P" };
            public int getSize() { return strings.length; }
            public Object getElementAt(int i) { return strings[i]; }
        });
        channelList.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        channelList.setCellRenderer(centeredCellRenderer);
        channelList.setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));
        channelList.setName("channelList"); // NOI18N
        channelList.addListSelectionListener(new javax.swing.event.ListSelectionListener() {
            public void valueChanged(javax.swing.event.ListSelectionEvent evt) {
                domesticCodeChanged(evt);
            }
        });

        buttonPanel.setName("buttonPanel"); // NOI18N

        org.jdesktop.layout.GroupLayout buttonPanelLayout = new org.jdesktop.layout.GroupLayout(buttonPanel);
        buttonPanel.setLayout(buttonPanelLayout);
        buttonPanelLayout.setHorizontalGroup(
            buttonPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(0, 471, Short.MAX_VALUE)
        );
        buttonPanelLayout.setVerticalGroup(
            buttonPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(0, 302, Short.MAX_VALUE)
        );

        org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .addContainerGap()
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(channelLabel)
                    .add(layout.createSequentialGroup()
                        .add(channelList, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 45, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                        .add(buttonPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(40, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .addContainerGap()
                .add(channelLabel)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING, false)
                    .add(channelList, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(buttonPanel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap(25, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void domesticCodeChanged(javax.swing.event.ListSelectionEvent evt) {//GEN-FIRST:event_domesticCodeChanged
        if ( evt.getValueIsAdjusting() == false )
            Options.getInstance().setDomesticCode(
                (String) channelList.getSelectedValue());
    }//GEN-LAST:event_domesticCodeChanged


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel buttonPanel;
    private javax.swing.JLabel channelLabel;
    private javax.swing.JList channelList;
    // End of variables declaration//GEN-END:variables

}
