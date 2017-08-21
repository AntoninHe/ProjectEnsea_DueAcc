#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:

    void about();
    void aboutQt();
    void helpQt();
    /* *******************************************************************************
     * Nom :        on_serialReceived
     * Description: fonction appelé lors de m'arrivée d'une donnée sur le port série.
     * Note :       On doit la temporiser pour permettre l'arrivé de toute les données.
     *              Sinon on ne lit que le début de la trame. */
    void on_serialReceived();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        on_readingprocess
     * Description: Cette fonction lit et filtre les donnés reçu
     *              Fontion déclanché par on_serialReceived avec un décalage de 5ms
     * Note :
     */
    void on_readingprocess();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        on_pushButtonClear_clicked
     * Description: reinitialise le tracé de la courbe
     * Note :
     */
    void on_pushButtonClear_clicked();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        on_pushButtonClose_clicked
     * Description: Ferme la liason série
     * Note : Cache les 3 boutons de commande
     */
    void on_pushButtonClose_clicked();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        on_pushButtonBreak_clicked
     * Description: Met en pause l'émission le timer de la carte Arduino
     * Note : envoi le caractère 'b'
     */
    void on_pushButtonBreak_clicked();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        Ouvre le port série
     * Description:
     * Note : Affiche les 3 boutons de commande
     */
    void on_pushButtonOpen_clicked();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        on_pushButtonStart_clicked
     * Description: démarre le timer de l'arduino et donc l'aquisition
     * Note : envoi le caractère 's'
     */
    void on_pushButtonStart_clicked();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        on_pushButtonFilter_clicked
     * Description: Permet le chargement d'un nouveau filtre
     * Note :       Cela suprimme et instancie de nouveau vecteur pour le traitemtent
     *              des données
     */
    void on_pushButtonFilter_clicked();
    /*********************************************************************************/




    void on_pushButtonNewStockage_clicked();

    void on_checkBoxExpOnoff_stateChanged(int arg1);

    void on_pushButtonExport_clicked();

    void on_verticalSlider_sliderMoved(int position);

    void on_pushButtonRefresh_clicked();

private:
    QAction *creditsAct;
    QAction *aboutQtAct;
    QAction *helpQtAct;

    Ui::MainWindow *ui;
    QList<QSerialPortInfo>  *infoportlist=NULL;//pour afficher la liste des ports disponible
    QString namefile;//stocke le nom du fichier de filtre
    QSerialPort *serialDue=NULL;//port série
    QTimer *readDelayTimer=NULL;//timer de temporisation
    std::vector<int> *tableFilter=NULL;//vecteur contenant le filtre
    std::vector<std::vector<int>> *instantvalueXYZ=NULL;//vecteur des donnés XYZ pour le filtrage
    std::vector<std::vector<int>> *stockage=NULL;
    void exportCSV();
    int exportvalue=0;

    int i_shift=1;//pour le buffer tournant/circulaire
    int icmpt=0;//nombre de points aquis
    /* *******************************************************************************
     * Nom :        filterLoad
     * Description: Charge le nouveau filtre et retoure le nouveau filtre
     * Note :       Si on ne charge pas de fichier cache les filtres
     */
    std::vector<int> *filterLoad();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        initialization_custumplot
     * Description: Init des l'affichage des courbes
     * Note :
     */
    void initialization_custumplot();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        initialization_serial
     * Description: Init des paramètres de la liaison série
     * Note :
     */
    void initialization_serial();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        initialization_tab
     * Description: initialisation des valeurs du filtre
     * Note :
     */
    void initialization_tab();

    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        initialization_timer
     * Description: Init du timer
     * Note :
     */
    void initialization_timer();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        fillPortsInfo
     * Description: rempli les ports dispo
     * Note :
     */
    void fillPortsInfo();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        update_tab
     * Description: met à jour les valeurs du filtres
     * Note :
     * */
    void update_tab();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        initialization_file
     * Description: Met a jour le fichier du filtre
     * Note :
     * */
    void initialization_file();
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        intsigned_to_int
     * Description: Convertit les données
     * Note :
     * */
    int intsigned_to_int(int);
    /*********************************************************************************/

    /* *******************************************************************************
     * Nom :        updateSettings
     * Description:
     * Note :
     * */
    void updateSettings();
    /*********************************************************************************/

    void initmenu();
};
#endif // MAINWINDOW_H
