# install.packages("hrbrthemes")
library(ggplot2)
library(dplyr)
library(patchwork) # To display 2 charts together
library(hrbrthemes)


get_results = function(local_path, alg_str) {
  results = array(rep(NaN, 3 * length(ps) * 2), c(3, length(ps), 2), dimnames = list(
    c("n=10^6", "n=10^7", "n=10^8"),
    c(),
    c("processors", "avg_time")
  ))
  
  for (ns_idx in 1:length(ns)) {
    n = ns[ns_idx]
    m = ms[ns_idx]
    for (p_idx in 1:length(ps)) {
      p_count = ps[p_idx]
      
      trial_file_path = sprintf(
        "%s/output_%s_%d_%d_%d.out",
        paste(job_outputs_path, local_path, sep = "/"),
        alg_str,
        n,
        m,
        p_count
      )
      
      trial_results = read.csv(trial_file_path)
      processors = trial_results[1, "p"]
      if (processors != p_count && alg_str != "seq") {
        stop(sprintf("p_count != processors: %d != %d", p_count, processors))
      }
      avg_time = mean(trial_results[, "time"])
      
      results[ns_idx, p_idx, 1] = processors
      results[ns_idx, p_idx, 2] = avg_time
    }
  }
  results <-
    provideDimnames(results ,
                    sep = "_",
                    base = list('input_size', 'p_count', 'x_y_point'))
}

get_seq_times = function(local_path) {
  seq_times_matrix = get_results(local_path, "seq")
  seq_times = vector(length = 3)
  for (i in 1:length(ns)) {
    seq_times[i] = mean(seq_times_matrix[i, , 1])
  }
  names(seq_times) = c("n=10^6", "n=10^7", "n=10^8")
  seq_times
}

get_speed_up = function(results_matrix, seq_times) {
  speed_up = results_matrix[, ,]
  for (i in 1:length(ns)) {
    speed_up[i, , 2] = rep(seq_times[i], length(speed_up[i, , 2])) / speed_up[i, , 2]
  }
  speed_up
}

make_graph = function(results_matrix,
                      seq_times,
                      title, 
                      saveFileBool) {
  for (ns_idx in 1:length(ns)) {
    
    speed_up = get_speed_up(results_matrix, seq_times)
  
    data_for_graph = data.frame(
      processors = results_matrix[ns_idx, , 1],
      avgtime = results_matrix[ns_idx, , 2],
      abs_speed_up = speed_up[ns_idx, , 2]
    )
    
    
    time_color = rgb(0.9, 0.6, 0.2, 1)
    asu_color =  rgb(0.2, 0.6, 0.9, 1)
    
    scale_coeff = max(data_for_graph["abs_speed_up"])/max(data_for_graph["avgtime"])
    
    # print(limits=data_for_graph["processors"])
    # break
     
    plot_to_draw = ggplot(data_for_graph, aes(x = processors)) +
      
      geom_line(aes(y = avgtime), size = 2, color = time_color) +
      geom_line(aes(y = abs_speed_up / scale_coeff),
                size = 2,
                color = asu_color) +
      
      scale_y_continuous(
        # Features of the first axis
        name = "Average Time (s)",
        limits = c(0, max(data_for_graph["avgtime"])),
        
        # Add a second axis and specify its features
        sec.axis = sec_axis( ~ . * scale_coeff, name = "Absolute Speed Up")
      ) +
      
      scale_x_discrete(name = "Processors", limits=ps) +
      
      
      theme(
        axis.title.x = element_text(color = "black", size = 13),
        axis.title.y = element_text(color = time_color, size = 13),
        axis.title.y.right = element_text(color = asu_color, size = 13)
      ) +
      
      ggtitle(sprintf(
        "%s: n = %s, m = %s",
        title,
        format(ns[ns_idx], big.mark = ",", scientific = FALSE),
        format(ms[ns_idx], big.mark = ",", scientific = FALSE)
      ))
    print(plot_to_draw)
    if (saveFileBool) {
      ggsave(sprintf("./merge_plot_%s_%d_%d.png", gsub(" ", "-", title), ns[ns_idx], ms[ns_idx]))  
    }
  }
}


setwd(dirname(rstudioapi::getActiveDocumentContext()$path))

job_outputs_path = "../parmerge-student-1.0.0-Source/job_outputs"

ns = c(10 ^ 6, 10 ^ 7, 10 ^ 8)
ms = c(2 * 10 ^ 6, 2 * 10 ^ 7, 2 * 10 ^ 8)
ps = c(2, 4, 8, 11, 16, 32)

seq_times = get_seq_times("seq/run_05-23-22[16_52_55]")

titles = c("Rank each element", "Co-rank", "Divide and Conquer")
merge_names = c("merge1", "merge2", "merge3")
merge_paths = c(
  "merge1/run_05-23-22[18_58_01]",
  "merge2/run_18_12_17",
  "merge3/run_06-11-22[15_31_25]_SWAP_TASKLOOP"
)

for (i in 1:length(merge_names)) {
  results = get_results(merge_paths[i], merge_names[i])
  make_graph(results, seq_times, titles[i], 0)
}

get_speed_up(get_results(merge_paths[1], merge_names[1]), seq_times)
