package src.networkBase;

import inet.networks.base.TsnNetworkBase;
import src.distribution.histogram.HistogramContainer;
//import src.common.binder.BinderTSN5G;

network TSN5GD6NetBase extends TsnNetworkBase // modulo para n ter q especificar em cada UseCase e ser mais facil de modificar
{
    parameters:
        //GlobalArp = true;
    submodules:
        histogramContainer: HistogramContainer {
            @display("p=100,771");
        }
}