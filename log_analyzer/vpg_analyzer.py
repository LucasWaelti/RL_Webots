import numpy as np 
import pandas as pd
from matplotlib import pyplot as plt 
import time 

from Analyzer import Analyzer

class Vpg_analyzer(Analyzer):
    '''
    Child class of Analyzer. 
    Implement functions specialized for the given subject. 
    '''

    def __init__(self, loc = '../controllers/vpg_controller/log/', file = 'log.csv'):
        super(Vpg_analyzer, self).__init__(loc, file)

    def results(self):
        fig, axes = plt.subplots(4,1,figsize=(12,8))
        fig.canvas.set_window_title('Statistics for {} rollouts'.format(self.m_data_df.shape[0]))

        # plot the loss
        sample_dim = 100 
        self._sub_plot_single(axes[0], 'Max_loss',legend='Max loss')
        self._sub_plot_single_from_array(axes[0], 
            self.compute_moving_average('Max_loss', sample_dim), 
            'rollouts', 'Max loss Moving Average (N={})'.format(sample_dim),legend='Max loss moving average')
        axes[0].legend()
        axes[0].grid()
        # Plot the cumulated reward 
        self._sub_plot_single_from_array(axes[1], 
            self.compute_moving_average('Cumulated_reward', sample_dim), 
            'rollouts', 'Cumulated Reward Moving Average (N={})'.format(sample_dim))
        axes[1].grid()
        # PLot the average success rate
        self._sub_plot_single_from_array(axes[2], 
            self.compute_moving_average('Distance', sample_dim), 
            'rollouts', 'Distance Moving Average (N={})'.format(sample_dim))
        axes[2].grid()
        # Plot the number of timesteps 
        self._sub_plot_single(axes[3], 'Num_timesteps') 

        fig.tight_layout()

if __name__ == "__main__":
    a = Vpg_analyzer(loc = '../controllers/vpg_controller/log/', file = 'log.csv')
    a.results()
    #a.plot_distance()
    a.show()

