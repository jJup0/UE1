library(ggplot2)
library(dplyr)
library(patchwork) # To display 2 charts together
library(hrbrthemes)




setwd(dirname(rstudioapi::getActiveDocumentContext()$path))

get_results_mv = function(local_path, alg_str, ps) {
  results = array(rep(NaN, length(ns) * 1 * 2), c(length(ns), 1, 2), dimnames = list(
    c("n=300", "n=600", "n=1200", "n=12000"),
    c(),
    c("processors", "avg_time")
  ))
  
  for (ns_idx in 1:length(ns)) {
    n = ns[ns_idx]
    
    for (p_idx in 1:length(ps)) {
      p_count = ps[p_idx]
      trial_file_path = sprintf(
        "%s/output_%s_%d_%d.out",
        paste(job_outputs_path, local_path, sep = "/"),
        alg_str,
        n,
        p_count
      )
      
      trial_results = read.csv(trial_file_path)
      
      processors = trial_results[1, "p"]
      if (processors != 8*p_count && alg_str != "seq") {
        stop(sprintf("p_count != processors: %d != %d", p_count, processors))
      }
      avg_time = mean(trial_results[, "t"])
      
      results[ns_idx, p_count, 1] = processors
      results[ns_idx, p_count, 2] = avg_time
    }
  }
  results =
    provideDimnames(results ,
                    sep = "_",
                    base = list('input_size', 'p_count', 'x_y_point'))
}


get_seq_times_mv = function(local_path, algname, ps) {
  seq_times_matrix = get_results_mv(local_path, algname, ps)
  seq_times = vector(length = 3)
  for (i in 1:length(ns)) {
    seq_times[i] = mean(seq_times_matrix[i, , 2])
  }
  names(seq_times) = c("n=10^6", "n=10^7", "n=10^8")
  return(seq_times)
}


get_speed_up = function(results_matrix, seq_times) {
  speed_up = results_matrix[, , ]
  for (i in 1:length(ns)) {
    speed_up[i, , 2] = rep(seq_times[i], length(speed_up[i, , 2])) / speed_up[i, , 2]
  }
  return(speed_up)
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
    
    scale_coeff = 1.1 * max(data_for_graph["abs_speed_up"]) / max(data_for_graph["avgtime"])
    
    print(data_for_graph["abs_speed_up"])
    
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
        sec.axis = sec_axis(~ . * scale_coeff, name = "Absolute Speed Up")
      ) +
      
      # scale_x_continuous(name = "Processors", breaks=ps) +
      scale_x_discrete(name = "Processors", limits = ps) +
      
      
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
      ggsave(sprintf(
        "./merge_plot_%s_%d_%d.png",
        gsub(" ", "-", title),
        ns[ns_idx],
        ms[ns_idx]
      ))
    }
  }
}


setwd(dirname(rstudioapi::getActiveDocumentContext()$path))

job_outputs_path = "../mpimv-student-1.0.0-Source/job_outputs"

ns = c(300, 600, 1200, 12000)
ps = c(1, 16, 32)

mv1_seq_times = get_seq_times_mv("mvseq/mv1_run_06-13-22[14_08_37]", "mv1_seq", c(1))
mv2_seq_times = get_seq_times_mv("mvseq/mv2_run_06-13-22[14_08_44]", "mv2_seq", c(1))

titles = c("MPI_Allgatherv",
           "MPI_Reduce_scatter")

mv_names = c("mv1",
             "mv2")

mv_paths = c("mv1/run_06-13-22[14_08_37]",
             "mv2/run_06-13-22[14_08_44]")

make_graph(get_results_mv(mv_paths[1], mv_names[1], ps),
           mv1_seq_times,
           titles[1],
           0)
make_graph(get_results_mv(mv_paths[2], mv_names[2], ps),
           mv2_seq_times,
           titles[2],
           0)

# get_speed_up(get_results(merge_paths[1], merge_names[1]), seq_times)
