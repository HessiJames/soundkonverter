
#include "icedaxripperglobal.h"

#include "soundkonverter_ripper_icedax.h"

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <KLocale>
#include <KComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QSlider>

soundkonverter_ripper_icedax::soundkonverter_ripper_icedax( QObject *parent, const QStringList& args  )
    : RipperPlugin( parent )
{
    Q_UNUSED(args)

    binaries["icedax"] = "";
}

soundkonverter_ripper_icedax::~soundkonverter_ripper_icedax()
{}

QString soundkonverter_ripper_icedax::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_ripper_icedax::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "audio cd";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["icedax"] != "" );
    newTrunk.data.canRipEntireCd = false;
    newTrunk.problemInfo = i18n( "In order to rip audio cds per track or to a single file, you need to install 'icedax'.\n'icedax' is usually shipped with your distribution, the package name can vary." );
    table.append( newTrunk );

    return table;
}

bool soundkonverter_ripper_icedax::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_ripper_icedax::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_ripper_icedax::hasInfo()
{
    return false;
}

void soundkonverter_ripper_icedax::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

unsigned int soundkonverter_ripper_icedax::rip( const QString& device, int track, int tracks, const KUrl& outputFile )
{
    QStringList command;

    command += binaries["icedax"];
    command += "-g";
    command += "-D";
    command += device;
    if( track > 0 )
    {
        command += "-t "+QString::number(track);
    }
    else
    {
        command += "-t 1+" + QString::number(tracks);
    }
    command += "\"" + outputFile.toLocalFile() + "\"";

    RipperPluginItem *newItem = new RipperPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    newItem->process->clearProgram();
    newItem->process->setShellCommand( command.join(" ") );
    newItem->process->start();

    logCommand( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

QStringList soundkonverter_ripper_icedax::ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile )
{
    Q_UNUSED(device)
    Q_UNUSED(track)
    Q_UNUSED(tracks)
    Q_UNUSED(outputFile)

    return QStringList();
}

float soundkonverter_ripper_icedax::parseOutput( const QString& output)
{
    QString data = output;
	data = data.left( data.lastIndexOf("%"));
	if (data.lastIndexOf("%")>=0)
		data = data.remove( 0, data.lastIndexOf("%"));
    data = data.simplified();
    return data.toFloat();
}

void soundkonverter_ripper_icedax::processOutput()
{
    RipperPluginItem *pluginItem;
    float progress;
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            QString output = backendItems.at(i)->process->readAllStandardOutput().data();
            pluginItem = qobject_cast<RipperPluginItem*>(backendItems.at(i));

            progress = parseOutput( output);

            if( !output.simplified().isEmpty() )
                logOutput( backendItems.at(i)->id, output );

            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;

            return;
        }
    }
}

#include "soundkonverter_ripper_icedax.moc"
