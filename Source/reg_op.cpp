

#include "reg_op.h"


StringArray readConfigFile()
{

    File file = File::getSpecialLocation( File::userApplicationDataDirectory ).getChildFile( "DonTouchMyMenu" ).getChildFile( "config.txt" );
    if( !file.existsAsFile() )
    {
        file.create();
    }

    auto t = file.loadFileAsString();
    StringArray k;
    k.addLines( t );

    StringArray x;
    for( auto e : k )
    {
        if( e.trim() == "" )
        {
            continue;
        }
        if( e.startsWith( "#" ) )
        {
            continue;
        }
        if( !WindowsRegistry::keyExists( e ) )
        {
            x.add( "key:" + e + " not exists" );
            continue;
        }
        auto result = WindowsRegistry::deleteKey( e );
        if( result )
        {
            x.add( "key:" + e + " deleted" );
        }
        else
        {
            x.add( "key:" + e + " failed to delete" );
        }
    }
    return x;
}
