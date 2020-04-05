import numpy as np 
import pandas as pd
from matplotlib import pyplot as plt 


class Analyzer:
    '''
    Base class for log analysis. 
    It provides tools for rapid analysis and plotting of the data. 
    '''
    def __init__(self, loc : str, file : str):
        ''' Init class members '''
        self.m_file_name = file 
        self.m_loc_name = loc
        self.read_log()
    
    ### Pandas -> #####################################################

    def read_log(self):
        self.m_data_df = pd.read_csv(self.m_loc_name+self.m_file_name, sep=',')
    
    def col_to_int_(self, columns:list):
        ''' Convert provided columns to int (in place). '''
        for c in columns:
            self.m_data_df[c] = self.m_data_df[c].apply(lambda x : int(x))

    def compute_quantity_rate(self, quantity:str, sample_dim=100):
        ''' Return the rates of a given quantity (DataFrame column) '''
        results = np.array(self.m_data_df[quantity].values)
        dim = len(results)
        rate = []
        for i in range(0,dim,sample_dim):
            if(i+sample_dim > dim):
                # Ignore computation for subset smaller than sample_dim
                break 
            sample = results[i:i+sample_dim]
            rate.append(np.sum(sample)/len(sample))
        return rate
    
    def compute_moving_average(self, quantity:str, sample_dim=100):
        ''' Return the rates of a given quantity (DataFrame column) as a moving average '''
        results = np.array(self.m_data_df[quantity].values)
        dim = len(results)
        rate = []
        for i in range(0,dim-sample_dim,1):
            #print('{} % done'.format(i/dim),end='\r')
            if(i+sample_dim > dim):
                # Ignore computation for subset smaller than sample_dim
                break 
            sample = results[i:i+sample_dim]
            rate.append(np.sum(sample)/len(sample))
        return rate

    def estimated_run_time(self):
        c = 'Num_timesteps'
        T = 0.008/10 # 8 ms, accelerated by ~23 ()fast mode
        return self.m_data_df[c].sum() * T / 3600 # Estimated run time in hour
        
    def show_head(self):
        print(self.m_data_df.head())

    ### -> Pandas / Pyplot -> ##########################################
    
    def _sub_plot_single(self, ax:plt.axis, column:str, legend=None):
        '''
        Create a sub-plot for a given column. 

        Parameters
        ----------
        ax : :plt.axis
            figure axis
        
        column : str
            column name
        '''
        if legend == None:
            ax.plot(self.m_data_df[column],linestyle='-',linewidth=0.1)
        else:
            ax.plot(self.m_data_df[column],linestyle='-',linewidth=0.1,label=legend)
        
        ax.set_xlabel('Rollouts')
        if False and column == 'Num_timesteps':
            column += ' (approx. run time: {0:.2f} hours)'.format(self.estimated_run_time())
        ax.set_title(column) 

    def _sub_plot_single_from_array(self, ax:plt.axis, data:np.array, x_title:str, title:str, legend=None):
        '''
        Create a sub-plot for a given column. 

        Parameters
        ----------
        ax : plt.axis
            figure axis
        
        data : np.array
            data to plot
        '''
        if legend == None:
            ax.plot(data,linestyle='-',linewidth=1)
        else:
            ax.plot(data,linestyle='-',linewidth=1,label=legend)
        ax.set_xlabel(x_title)
        ax.set_title(title) 

    def _sub_plot_multiple(self, axis:plt.axis, columns:tuple):
        '''
        Create a sub-plot for the given columns. 

        Parameters
        ----------
        ax : plt.axis
            figure axis
        columns : tuple
            column names
        '''
        title = ''
        for col in columns:
            axis.plot(self.m_data_df[col], label=col,linestyle='-',linewidth=0.1)
            title += col + ', '
        axis.set_title(title) 
        axis.set_xlabel('Rollouts')
        axis.legend()

    def _sub_plot_multiple_twin(self, axis:plt.axis, columns:tuple):
        '''
        Create a sub-plot for the given columns. 
        Create a twin x-axis with new y scale for the second column. 

        Parameters
        ----------
        ax : plt.axis
            figure axis
        column : tuple
            2 column names
        twin : bool
            
        '''
        # PLot first column
        axis.plot(self.m_data_df[columns[0]], label=columns[0], color='tab:blue',linestyle='-',linewidth=0.1)
        axis.tick_params(axis='y', labelcolor='tab:blue')
        axis.set_xlabel('Rollouts')
        axis.set_ylabel(columns[0])
        axis.set_title(columns[0] + ' ~ ' + columns[1])

        # PLot second column 
        ax = axis.twinx()
        ax.plot(self.m_data_df[columns[1]], label=columns[1], color='tab:red',linestyle='-',linewidth=1)
        ax.set_ylabel(columns[1])
        ax.tick_params(axis='y', labelcolor='tab:red')

    def show(self):
        plt.tight_layout()
        plt.show()

    def save(self, file = "fig.png"):
        plt.savefig(file, bbox_inches='tight') 
    
    def close_all(self):
        plt.close('all')



