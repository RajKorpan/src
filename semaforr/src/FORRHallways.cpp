#include<FORRHallways.h>

using namespace std;



//----------------------//------------------------//



void FORRHallways::CreateSegments(vector<Segment> &segments, const vector<CartesianPoint> &trails, const vector < vector <CartesianPoint> > &laser_history) {
  cout << "num of trail markers " << trails.size() << " num of laser history " << laser_history.size() << endl;
  for (int i = 0; i < trails.size()-1; i++){
    //if(agent_state->canAccessPoint(laser_history[i], trails[i], trails[i+1]) or agent_state->canAccessPoint(laser_history[i+1], trails[i+1], trails[i])){
    Segment current_segment = Segment(trails[i], trails[i+1], laser_history[i], laser_history[i+1]);
    if(trails[i].get_x() > trails[i+1].get_x())
      current_segment = Segment(trails[i+1], trails[i], laser_history[i+1], laser_history[i]);
    if((trails[i].get_x() != trails[i+1].get_x()) and (trails[i].get_y() != trails[i+1].get_y()) and (pow((pow((trails[i].get_x() - trails[i+1].get_x()), 2)+pow((trails[i].get_y() - trails[i+1].get_y()), 2)),0.5)>=0.5))
      segments.push_back(current_segment);
    //}
  }
}



//----------------------//------------------------//



// find min and max for each row (x0, y0, x1, y1, angle)
void FORRHallways::NormalizeVector(vector<vector<double> > &normalized_segments, const vector<Segment> &segments) {
  vector<vector<double> > segments_data;
  ConvertSegmentsToDouble(segments_data, segments);
  for(int i = 0; i < segments_data[0].size(); i++) {
    double min, max;

    min = FindMin(segments_data, i);
    max = FindMax(segments_data, i);

    //cout << "min: " << min << " " << "max: " << max << endl;

    for(int j = 0; j < segments_data.size(); j++) {
      normalized_segments[j][i] = Normalize(segments_data[j][i], min, max); // be careful with matrix numbering here
      //Normalize(segments_data[j][i], min, max);
    }
  }
}

void FORRHallways::ConvertSegmentsToDouble(vector<vector<double> > &data, const vector<Segment> &segments) {
  vector<double> single_segment_data;
  for(int i = 0; i< segments.size(); i++) {
    single_segment_data.push_back(segments[i].GetLeftPoint().get_x());
    single_segment_data.push_back(segments[i].GetLeftPoint().get_y());
    single_segment_data.push_back(segments[i].GetRightPoint().get_x());
    single_segment_data.push_back(segments[i].GetRightPoint().get_y());
    single_segment_data.push_back(segments[i].GetAngle());
    data.push_back(single_segment_data);
    single_segment_data.clear();
  }
}

// assumes vector has at least 1 vector w/ 1 value
//TODO:: very similar to FindMax
// how to set functions equal to each other
// to set up 2 boolean conditions for min/max
// reduce this so only a 1-D vector is passed
double FORRHallways::FindMin(const vector<vector<double> > &segments, const int pos) {
  double min = segments[0][pos];
  for(int i = 0; i < segments.size(); i++) {
    if(segments[i][pos] < min)
      min = segments[i][pos];
  }
  return min;
}

// assumes vector has at least 1 vector w/ 1 value
double FORRHallways::FindMax(const vector<vector<double> > &segments, const int pos) {
  double max = segments[0][pos];
  for(int i = 0; i < segments.size(); i++) {
    if(segments[i][pos] > max)
      max = segments[i][pos];
  }
  return max;
}

//input:
//output:
//TODO: what if min = max
double FORRHallways::Normalize(const double original_val, const double min_val, const double max_val) {
  if(max_val != min_val){
  	return (original_val - min_val)/(max_val - min_val);
  }
  else{
  	return 0.0;
  }
}



//----------------------//------------------------//



void FORRHallways::ListSimilarities(vector<vector<double> > &similarities, const vector<vector<double> > &normalized_segments) {
  vector<double> segments_and_similarities;

  for(int i = 0; i < normalized_segments.size() - 1; i++) {
    for(int j = i + 1; j < normalized_segments.size(); j++) {
      segments_and_similarities.push_back(double(i));
      segments_and_similarities.push_back(double(j));
      segments_and_similarities.push_back(ComputeDistance(normalized_segments[i], normalized_segments[j]));
      similarities.push_back(segments_and_similarities);
      segments_and_similarities.clear();
    }
  }
}

//input: two vectors of 2 coordinates each, type double
//output: sum of squared differences, type double
//TODO:: should name be changed to reflect squared nature?
//TODO:: parameterize nth root
double FORRHallways::ComputeDistance(const vector<double> first_segment, const vector<double> second_segment) {
  /*double sum = 0;
  for(int i = 0; i < first_segment.size(); i++) {
    sum += pow((first_segment[i] - second_segment[i]), 2);
  }
  sum = pow(sum, .5); //remember it's rooted in matlab function;*/
  //double dist1 = pow((pow((first_segment[0] - second_segment[0]), 2) + pow((first_segment[1] - second_segment[1]), 2)), 0.5);
  //double dist2 = pow((pow((first_segment[2] - second_segment[2]), 2) + pow((first_segment[3] - second_segment[3]), 2)), 0.5);
  double first_avg_x = (first_segment[0] + first_segment[2])/2;
  double first_avg_y = (first_segment[1] + first_segment[3])/2;
  double second_avg_x = (second_segment[0] + second_segment[2])/2;
  double second_avg_y = (second_segment[1] + second_segment[3])/2;
  double dist = pow((pow((first_avg_x - second_avg_x), 2) + pow((first_avg_y - second_avg_y), 2)), 0.5);
  double angledist = max(first_segment[4],second_segment[4]) - min(first_segment[4],second_segment[4]);
  if(angledist > M_PI/2){
    angledist = M_PI - (angledist);
  }
  //double angledist = pow((pow((first_segment[4] - second_segment[4]), 2)), 0.5);
  //double sum = (0.25*dist1 + 0.25*dist2 + 0.5*angledist);
  //cout << first_segment[0] << " " << first_segment[1] << " " << first_segment[2] << " " << first_segment[3] << " " << first_avg_x << " " << first_avg_y << endl;
  //cout << second_segment[0] << " " << second_segment[1] << " " << second_segment[2] << " " << second_segment[3] << " " << second_avg_x << " " << second_avg_y << endl;
  //cout << dist << " " << first_segment[4] << " " << second_segment[4] << " " << angledist << endl;
  double sum = (dist + angledist)/2;
  return sum;
}



//----------------------//------------------------//



void FORRHallways::FindMostSimilarSegments(vector<vector<double> > &most_similar, const vector<vector<double> > &similarities) {
  double average_of_distances, sum_of_distances, std, sum_of_squared_differences, threshold = 0;

  for(int i = 0; i < similarities.size(); i++) {
    sum_of_distances += similarities[i][2]; // similarity score
  }
  average_of_distances = sum_of_distances / similarities.size();
  //cout << "average_of_distances = " << average_of_distances << endl;
  for(int i = 0; i < similarities.size(); i++) {
    sum_of_squared_differences += pow((similarities[i][2]-average_of_distances), 2);
  }

  double normalized_sum_of_squared_differences = sum_of_squared_differences/(similarities.size());
  std = pow(normalized_sum_of_squared_differences, .5); // square root of squared difference sum
  //cout << "std = " << std << endl;
  double deviations = 3.5;
  if(isinf(std) == false){
    while(most_similar.size() == 0 and deviations > 0){
      deviations = deviations-0.25;
      threshold = average_of_distances - (deviations*std);
      //cout << "threshold " << threshold << endl;
      for(int i = 0; i < similarities.size(); i++) {
        //cout << similarities[i][2] << endl;
        if(similarities[i][2] <= threshold) {
          vector<double> similar_pairing;
          similar_pairing.push_back(similarities[i][0]);
          similar_pairing.push_back(similarities[i][1]);
          most_similar.push_back(similar_pairing);
          //cout << similarities[i][0] << " " << similarities[i][1] << " " << similarities[i][2] << endl;
          similar_pairing.clear();
        }
      }
    }
  }
}



//----------------------//------------------------//



void FORRHallways::CreateMeanSegments(vector<Segment> &averaged_segments, const vector<vector<double> > &most_similar, const vector<Segment> &segments, double step) {
  double average_left_x, average_left_y, average_right_x, average_right_y, average_left_right_x, average_left_right_y, average_right_left_x, average_right_left_y = 0;
  for(int i = 0; i < most_similar.size(); i++) {
    Segment first = segments[int(most_similar[i][0])];
    Segment second = segments[int(most_similar[i][1])];
    average_left_x = (first.GetLeftPoint().get_x() + second.GetLeftPoint().get_x())/2;
    average_left_y = (first.GetLeftPoint().get_y() + second.GetLeftPoint().get_y())/2;
    CartesianPoint left_coord = CartesianPoint(average_left_x, average_left_y);

    average_right_x = (first.GetRightPoint().get_x() + second.GetRightPoint().get_x())/2;
    average_right_y = (first.GetRightPoint().get_y() + second.GetRightPoint().get_y())/2;
    CartesianPoint right_coord = CartesianPoint(average_right_x, average_right_y);
    Segment average_same = Segment(left_coord, right_coord);
    if(average_left_x > average_right_x){
      average_same = Segment(right_coord, left_coord);
    }

    average_left_right_x = (first.GetLeftPoint().get_x() + second.GetRightPoint().get_x())/2;
    average_left_right_y = (first.GetLeftPoint().get_y() + second.GetRightPoint().get_y())/2;
    CartesianPoint left_right_coord = CartesianPoint(average_left_right_x, average_left_right_y);

    average_right_left_x = (first.GetRightPoint().get_x() + second.GetLeftPoint().get_x())/2;
    average_right_left_y = (first.GetRightPoint().get_y() + second.GetLeftPoint().get_y())/2;
    CartesianPoint right_left_coord = CartesianPoint(average_right_left_x, average_right_left_y);
    Segment average_diff = Segment(left_right_coord, right_left_coord);
    if(average_left_right_x > average_right_left_x){
      average_diff = Segment(right_left_coord, left_right_coord);
    }
    Segment average = average_same;
    double average_same_angle = average_same.GetAngle() * 180/M_PI;
    double average_diff_angle = average_diff.GetAngle() * 180/M_PI;
    //cout << "first angle = " << first.GetAngle() << " second angle = " << second.GetAngle() << " average_same_angle = " << average_same.GetAngle() << " average_diff_angle = " << average_diff.GetAngle() << endl;
    bool match = false;
    if(first.GetAngle() * 180/M_PI <= step and first.GetAngle() * 180/M_PI >= 0 and second.GetAngle() * 180/M_PI <= step and second.GetAngle() * 180/M_PI >= 0){
      if(average_same_angle <= step and average_same_angle >= 0){
        average = average_same;
        match = true;
      }
      else if(average_diff_angle <= step and average_diff_angle >= 0){
        average = average_diff;
        match = true;
      }
    }
    else if(first.GetAngle() * 180/M_PI <= 45+step and first.GetAngle() * 180/M_PI >= 45-step and second.GetAngle() * 180/M_PI <= 45+step and second.GetAngle() * 180/M_PI >= 45-step){
      if(average_same_angle <= 45+step and average_same_angle >= 45-step){
        average = average_same;
        match = true;
      }
      else if(average_diff_angle <= 45+step and average_diff_angle >= 45-step){
        average = average_diff;
        match = true;
      }
    }
    else if(first.GetAngle() * 180/M_PI <= 90+step and first.GetAngle() * 180/M_PI >= 90-step and second.GetAngle() * 180/M_PI <= 90+step and second.GetAngle() * 180/M_PI >= 90-step){
      if(average_same_angle <= 90+step and average_same_angle >= 90-step){
        average = average_same;
        match = true;
      }
      else if(average_diff_angle <= 90+step and average_diff_angle >= 90-step){
        average = average_diff;
        match = true;
      }
    }
    else if(first.GetAngle() * 180/M_PI <= 135+step and first.GetAngle() * 180/M_PI >= 135-step and second.GetAngle() * 180/M_PI <= 135+step and second.GetAngle() * 180/M_PI >= 135-step){
      if(average_same_angle <= 135+step and average_same_angle >= 135-step){
        average = average_same;
        match = true;
      }
      else if(average_diff_angle <= 135+step and average_diff_angle >= 135-step){
        average = average_diff;
        match = true;
      }
    }
    else if(first.GetAngle() * 180/M_PI <= 180 and first.GetAngle() * 180/M_PI >= 180-step and second.GetAngle() * 180/M_PI <= 180 and second.GetAngle() * 180/M_PI >= 180-step){
      if(average_same_angle <= 180 and average_same_angle >= 180-step){
        average = average_same;
        match = true;
      }
      else if(average_diff_angle <= 180 and average_diff_angle >= 180-step){
        average = average_diff;
        match = true;
      }
    }
    //double angledist_same = (pow((pow((average_same.GetAngle() - first.GetAngle()), 2)), 0.5) + pow((pow((average_same.GetAngle() - second.GetAngle()), 2)), 0.5));
    //double angledist_diff = (pow((pow((average_diff.GetAngle() - first.GetAngle()), 2)), 0.5) + pow((pow((average_diff.GetAngle() - second.GetAngle()), 2)), 0.5));
    //if(angledist_same > angledist_diff){
    //  average = average_diff;
    //}
    if(match == true and agent_state->canAccessPoint(first.GetLeftLaser(), first.GetLeftPoint(), average.GetLeftPoint(), 20) and agent_state->canAccessPoint(second.GetLeftLaser(), second.GetLeftPoint(), average.GetLeftPoint(), 20) and agent_state->canAccessPoint(first.GetRightLaser(), first.GetRightPoint(), average.GetRightPoint(), 20) and agent_state->canAccessPoint(second.GetRightLaser(), second.GetRightPoint(), average.GetRightPoint(), 20)){
      //cout << average.GetAngle() << endl;
      averaged_segments.push_back(average);
      averaged_segments.push_back(first);
      averaged_segments.push_back(second);
      //average.PrintSegment();
      //first.PrintSegment();
      //second.PrintSegment();
    }
    //cout << first.GetLeftPoint().get_x() << " " << first.GetLeftPoint().get_y() << " " << first.GetRightPoint().get_x() << " " << first.GetRightPoint().get_y() << " " << first.GetAngle() << " " << second.GetLeftPoint().get_x() << " " << second.GetLeftPoint().get_y() << " " << second.GetRightPoint().get_x() << " " << second.GetRightPoint().get_y() << " " << second.GetAngle() << " " << average.GetLeftPoint().get_x() << " " << average.GetLeftPoint().get_y() << " " << average.GetRightPoint().get_x() << " " << average.GetRightPoint().get_y() << " " << average.GetAngle() << endl;
  }
}

//----------------------//------------------------//

// filtered line is one below than expected pos
// just pass 1 vector of hallway
vector<vector<CartesianPoint> > FORRHallways::ProcessHallwayData(const vector<Segment> &hallway_group, int width, int height, double threshold) {
    vector<vector<double> > heat_map(width,vector<double>(height, 0));
    vector<vector<double> > smoothed_heat_map(width,vector<double>(height, 0));
    vector<vector<double> > filtered_heat_map(width,vector<double>(height, 0));
    vector<vector<int> > binarized_heat_map(width,vector<int>(height, 0));
    vector<vector<int> > labeled_image(width,vector<int>(height, 0));
    
    //cout << heat_map.size() << " " << heat_map[0].size() << endl;
    //cout << hallway_groups[i].size() << endl;
    UpdateMap(heat_map, hallway_group);
    //cout << "1a" << endl;

    SmoothMap(smoothed_heat_map, heat_map, threshold);
    //cout << "2a" << endl;
    /*FilterImage(filtered_heat_map,heat_map,9);

    BinarizeImage(binarized_heat_map, filtered_heat_map, 0);*/
    //BinarizeImage(binarized_heat_map, heat_map, 1);
    BinarizeImage(binarized_heat_map, smoothed_heat_map, threshold);

    //cout << "3a" << endl;
    LabelImage(binarized_heat_map,labeled_image);

    //cout << "3c" << endl; //error1
    vector<vector< pair<int,int> > > points_in_aggregates;
    ListGroups(points_in_aggregates, labeled_image);
    //cout << "3d" << endl;

    vector<vector<CartesianPoint> > points_in_groups;
    ConvertPairToCartesianPoint(points_in_groups, points_in_aggregates);
    //cout << "3e" << endl; //error1

    return points_in_groups;
}

// isnt accurate to line
// also slow
void FORRHallways::UpdateMap(vector<vector<double> > &frequency_map, const vector<Segment> &segments) {
  //cout << "map size input: " << frequency_map.size() << " " << frequency_map[0].size() << endl;
  //cout << segments.size() << endl;
  for(int i = 0; i < segments.size(); i++) {
    //cout << i << endl;
    CartesianPoint left = segments[i].GetLeftPoint();
    //cout << i << "left" << endl;
    CartesianPoint right = segments[i].GetRightPoint();
    //cout << i << "right" << endl;
    Interpolate(frequency_map, round(left.get_x()), round(left.get_y()), round(right.get_x()), round(right.get_y()));
    //cout << i <<"Done"<< endl;
  }
  /*cout << "Heat Map" << endl;
  for(int i = 0; i < frequency_map.size(); i++) {
    for(int j = 0; j < frequency_map[0].size(); j++) {
      cout << frequency_map[i][j] << " ";
    }
    cout << endl;
  }*/
}

//********$^$^^%#^#

// does this produce a similar map?
void FORRHallways::Interpolate(vector<vector<double> > &frequency_map, double left_x, double left_y, double right_x, double right_y) {

  double step = abs(left_x - right_x) + abs(left_y - right_y);
  //cout << "step: " << step << " " << 1/step << endl;

  for(double j = 0; j <= 1; j += 1/step) {
    //cout << j << endl;
    int xcoord = round(j*(right_x - left_x) + left_x);
    int ycoord = round(j*(right_y - left_y) + left_y);
    //cout << xcoord << " " << ycoord << endl;
    //cout << frequency_map[0].size() << " " << frequency_map.size() << endl;
    frequency_map[xcoord][ycoord] += 1.0;
    //cout << xcoord << " " << ycoord << " " << frequency_map[xcoord][ycoord] << endl;
    /*if(xcoord>0 and ycoord>0){
      frequency_map[xcoord-1][ycoord-1] += 0.1;
    }
    if(xcoord>0){
      frequency_map[xcoord-1][ycoord] += 0.1;
    }
    if(ycoord>0){
      frequency_map[xcoord][ycoord-1] += 0.1;
    }
    if(xcoord < frequency_map.size() and ycoord < frequency_map[0].size()){
      frequency_map[xcoord+1][ycoord+1] += 0.1;
    }
    if(xcoord < frequency_map.size()){
      frequency_map[xcoord+1][ycoord] += 0.1;
    }
    if(ycoord < frequency_map[0].size()){
      frequency_map[xcoord][ycoord+1] += 0.1;
    }
    if(xcoord>0 and ycoord < frequency_map[0].size()){
      frequency_map[xcoord-1][ycoord+1] += 0.1;
    }
    if(xcoord < frequency_map.size() and ycoord>0){
      frequency_map[xcoord+1][ycoord-1] += 0.1;
    }*/
  }
}


void FORRHallways::SmoothMap(vector<vector<double> > &frequency_map, const vector<vector<double> > &heat_map, double threshold) {
  for(int i = 0; i < heat_map.size(); i++) {
    for(int j = 0; j < heat_map[0].size(); j++) {
      //cout << i << "," << j << " " << heat_map[i][j] << endl;
      double value = heat_map[i][j];
      if(value > 0){
        frequency_map[i][j] = value;
      }
      else{
        double count = 0;
        if(i>0){
          if(heat_map[i-1][j]>0)
            value++;
          count++;
        }
        if(i>0 and j>0){
          if(heat_map[i-1][j-1]>0)
            value++;
          count++;
        }
        if(i>0 and j<heat_map[0].size()-1){
          if(heat_map[i-1][j+1]>0)
            value++;
          count++;
        }
        if(i<heat_map.size()-1 and j>0){
          if(heat_map[i+1][j-1]>0)
            value++;
          count++;
        }
        if(i<heat_map.size()-1){
          if(heat_map[i+1][j]>0)
            value++;
          count++;
        }
        if(i<heat_map.size()-1 and j<heat_map[0].size()-1){
          if(heat_map[i+1][j+1]>0)
            value++;
          count++;
        }
        if(j>0){
          if(heat_map[i][j-1]>0)
            value++;
          count++;
        }
        if(j<heat_map[0].size()-1){
          if(heat_map[i][j+1]>0)
            value++;
          count++;
        }
        frequency_map[i][j] = value/count;
      }
    }
  }
  bool change_made = true;
  while(change_made == true){
    int num_changes = 0;
    for(int i = 0; i < frequency_map.size(); i++) {
      for(int j = 0; j < frequency_map[0].size(); j++) {
        double value = 0;
        double count = 0;
        if(frequency_map[i][j] < 1){
          if(i>0){
            if(frequency_map[i-1][j]>=1)
              value++;
            count++;
          }
          if(i>0 and j>0){
            if(frequency_map[i-1][j-1]>=1)
              value++;
            count++;
          }
          if(i>0 and j<frequency_map[0].size()-1){
            if(frequency_map[i-1][j+1]>=1)
              value++;
            count++;
          }
          if(i<frequency_map.size()-1 and j>0){
            if(frequency_map[i+1][j-1]>=1)
              value++;
            count++;
          }
          if(i<frequency_map.size()-1){
            if(frequency_map[i+1][j]>=1)
              value++;
            count++;
          }
          if(i<frequency_map.size()-1 and j<frequency_map[0].size()-1){
            if(frequency_map[i+1][j+1]>=1)
              value++;
            count++;
          }
          if(j>0){
            if(frequency_map[i][j-1]>=1)
              value++;
            count++;
          }
          if(j<frequency_map[0].size()-1){
            if(frequency_map[i][j+1]>=1)
              value++;
            count++;
          }
          if(value/count >= threshold){
            //cout << "value " << value << " count " << count << endl;
            frequency_map[i][j] = 1;
            num_changes++;
          }
        }
      }
    }
    //cout << "num_changes " << num_changes << endl;
    if(num_changes == 0){
      change_made = false;
    }
  }
  /*cout << "Smoothed Heat Map" << endl;
  for(int i = 0; i < frequency_map.size(); i++) {
    for(int j = 0; j < frequency_map[0].size(); j++) {
      cout << frequency_map[i][j] << " ";
    }
    cout << endl;
  }*/
}


//TODO: MATLAB filter specifically for radius of 9, and all values
vector<vector<int> > FORRHallways::CreateCircularAveragingFilter(int radius) {
  vector<vector<int> > filter((2*radius) + 1,vector<int>((2*radius)+1, 0));
  for(int i = 0; i < filter.size(); i++) {
    for(int j = 0; j < filter[i].size(); j++) {
      if( pow((i-radius), 2) + pow((j-radius), 2)  <= pow(radius,2))
    filter[i][j] = 1;
    }
  }
  return filter;
}

vector<vector<double> > FORRHallways::ReturnMatlabFilter() {
  vector<vector<double> > filter;
  double arr1[] = {0,0,0,0,0,0,0.00013527,0.0010609,0.0017273,0.0019467,0.0017273,0.0010609,0.00013527,0,0,0,0,0,0};
  std::vector<double> v1(arr1, arr1+19);
  filter.push_back(v1);
  double arr2[19] = {0,0,0,0,0.00028092,0.0021842,0.0037149,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0037149,0.0021842,0.00028092,0,0,0,0};
  std::vector<double> v2(arr2, arr2+19);
  filter.push_back(v2);
  double arr3[19] = {0,0,0,0.00092058,0.0035515,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0035515,0.00092058,0,0,0};
  std::vector<double> v3(arr3, arr3+19);
  filter.push_back(v3);
  double arr4[19] = {0,0,0.00092058,0.0037832,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0037832,0.00092058,0,0};
  std::vector<double> v4(arr4, arr4+19);
  filter.push_back(v4);
  double arr5[19] = {0,0.00028092,0.0035515,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0035515,0.00028092,0};
  std::vector<double> v5(arr5, arr5+19);
  filter.push_back(v5);
  double arr6[19] = {0,0.0021842,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0021842,0};
  std::vector<double> v6(arr6, arr6+19);
  filter.push_back(v6);
  double arr7[19] = {0.00013527,0.0037149,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0037149,0.00013527};
  std::vector<double> v7(arr7, arr7+19);
  filter.push_back(v7);
  double arr8[19] = {0.0010609,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0010609};
  std::vector<double> v8(arr8, arr8+19);
  filter.push_back(v8);
  double arr9[19] = {0.0017273,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0017273};
  std::vector<double> v9(arr9, arr9+19);
  filter.push_back(v9);
  double arr10[19] = {0.0019467,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0019467};
  std::vector<double> v10(arr10, arr10+19);
  filter.push_back(v10);
  double arr11[19] = {0.0017273,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0017273};
  std::vector<double> v11(arr11, arr11+19);
  filter.push_back(v11);
  double arr12[19] = {0.0010609,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0010609};
  std::vector<double> v12(arr12, arr12+19);
  filter.push_back(v12);
  double arr13[19] = {0.00013527,0.0037149,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0037149,0.00013527};
  std::vector<double> v13(arr13, arr13+19);
  filter.push_back(v13);
  double arr14[19] = {0,0.0021842,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0021842,0};
  std::vector<double> v14(arr14, arr14+19);
  filter.push_back(v14);
  double arr15[19] = {0,0.00028092,0.0035515,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0035515,0.00028092,0};
  std::vector<double> v15(arr15, arr15+19);
  filter.push_back(v15);
  double arr16[19] = {0,0,0.00092058,0.0037832,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0037832,0.00092058,0,0};
  std::vector<double> v16(arr16, arr16+19);
  filter.push_back(v16);
  double arr17[19] = {0,0,0,0.00092058,0.0035515,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0035515,0.00092058,0,0,0};
  std::vector<double> v17(arr17, arr17+19);
  filter.push_back(v17);
  double arr18[19] = {0,0,0,0,0.00028092,0.0021842,0.0037149,0.0039298,0.0039298,0.0039298,0.0039298,0.0039298,0.0037149,0.0021842,0.00028092,0,0,0,0};
  std::vector<double> v18(arr18, arr18+19);
  filter.push_back(v18);
  double arr19[19] = {0,0,0,0,0,0,0.00013527,0.0010609,0.0017273,0.0019467,0.0017273,0.0010609,0.00013527,0,0,0,0,0,0};
  std::vector<double> v19(arr19, arr19+19);
  filter.push_back(v19);
  return filter;
}

// must produce double value matrix


// assumes matrix is odd sized square
// first inner loop includes 0 now
// why is everything one line below?
// and one line over...
void FORRHallways::FilterImage(vector<vector<double> > &filtered, const vector<vector<int> > &original, int radius) {

  //vector<vector<int> > averaging_filter = createCircularAveragingFilter(radius);
  vector<vector<double> > averaging_filter = ReturnMatlabFilter();

  int matrix_radius  = (averaging_filter.size() - 1)/2;
  //create 19x19 vector all 0s

  vector<vector<double> > matrix_piece(averaging_filter.size(), vector<double>(averaging_filter.size(), 0));
  int num_rows = original[0].size();
  int num_columns = original.size();

  for(int i = 0; i < original[0].size(); i++) {
    for(int j = 0; j < original.size(); j++) {

      for(int n = -matrix_radius; n <= matrix_radius; n++) {
        for(int m = -matrix_radius; m <= matrix_radius; m++) {
          if((i+m >= 0 && (i+m <= num_rows-1)) && (j+n >= 0 && (j+n <= num_columns-1))) {
            matrix_piece[n+matrix_radius][m+matrix_radius] = static_cast<double>(original[j+n][i+m]);
          }
          else {
            matrix_piece[n+matrix_radius][m+matrix_radius] = 0;
          }
        }
      }


      double hold = 0;
      for(int k = 0; k < averaging_filter.size(); k++) {
        for(int u = 0; u < averaging_filter.size(); u++) {
          hold += matrix_piece[k][u]*averaging_filter[k][u];
        }
      }
      filtered[j][i] = hold;
    }
  }
}
  // assumes binarized has the same dimensions as original
void FORRHallways::BinarizeImage(vector<vector<int> > &binarized, const vector<vector<double> > &original, double threshold) {
  for(int i = 0; i < original.size(); i++) {
    for(int j = 0; j < original[0].size(); j++) {
      if(original[i][j] >= threshold){
        binarized[i][j] = 255;
        /*if(i>0 and j>0){
          binarized[i-1][j-1] = 255;
        }
        if(i>0){
          binarized[i-1][j] = 255;
        }
        if(j>0){
          binarized[i][j-1] = 255;
        }
        if(i < original.size() and j < original[0].size()){
          binarized[i+1][j+1] = 255;
        }
        if(i < original.size()){
          binarized[i+1][j] = 255;
        }
        if(j < original[0].size()){
          binarized[i][j+1] = 255;
        }
        if(i>0 and j < original[0].size()){
          binarized[i-1][j+1] = 255;
        }
        if(i < original.size() and j>0){
          binarized[i+1][j-1] = 255;
        }*/
      }
      //cout << binarized[i][j] << " ";
    }
    //cout << endl;
  }
}


void FORRHallways::doUnion(int x, int y, int x2, int y2, vector<vector<int> > &labeled_image){
  int a = map_width_*y+x;
  int ya = y;
  int xa = x;
  //cout << "a " << a << " " << xa << " " << ya << endl;
  while (labeled_image[xa][ya] != a){
    a = labeled_image[xa][ya];
    ya = floor(a/map_width_);
    xa = a - map_width_*ya;
    //cout << a << " " << xa << " " << ya << endl;
  }
  int b = map_width_*y2+x2;
  int yb = y2;
  int xb = x2;
  //cout << "b " << b << " " << xb << " " << yb << endl;
  while (labeled_image[xb][yb] != b){
    b = labeled_image[xb][yb];
    yb = floor(b/map_width_);
    xb = b - map_width_*yb;
    //cout << b << " " << xb << " " << yb << endl;
  }
  labeled_image[xb][yb] = a;
}
 
void FORRHallways::unionCoords(int x, int y, int x2, int y2, const vector<vector<int> > &binary_map, vector<vector<int> > &labeled_image){
  if (y2 < map_height_ && x2 < map_width_ && binary_map[x][y] && binary_map[x2][y2]){
    //cout << y2 << " " << x2 << " " << binary_map[x][y] << " " << binary_map[x2][y2] << endl;
    doUnion(x, y, x2, y2, labeled_image);
  }
}

void FORRHallways::LabelImage(const vector<vector<int> > &binary_map, vector<vector<int> > &labeled_image){
  //cout << "map_width_ " << map_width_ << " map_height_ " << map_height_ << endl;
  for (int x = 0; x < map_width_; x++){
    for (int y = 0; y < map_height_; y++){
      labeled_image[x][y] = map_width_*y+x;
      //cout << x << " " << y << " " << map_width_*y+x << endl;
    }
  }

  for (int x = 0; x < map_width_; x++){
    for (int y = 0; y < map_height_; y++){
      unionCoords(x, y, x+1, y, binary_map, labeled_image);
      unionCoords(x, y, x, y+1, binary_map, labeled_image);
      unionCoords(x, y, x+1, y+1, binary_map, labeled_image);
    }
  }
  for (int x = 0; x < map_width_; x++){
    for (int y = 0; y < map_height_; y++){
      if (binary_map[x][y] == 0){
        //cout << ' ';
        labeled_image[x][y] = -1;
        continue;
      }
      int c = map_width_*y+x;
      int xnew = x;
      int ynew = y;
      while (labeled_image[xnew][ynew] != c){
        c = labeled_image[xnew][ynew];
        ynew = floor(c/map_width_);
        xnew = c - map_width_*ynew;
      }
      //cout << c << " ";
      labeled_image[x][y] = c;
    }
    //cout << "\n";
  }
}

void FORRHallways::ListGroups(vector<vector< pair<int,int> > > &aggregates_and_points, const vector<vector<int> > &labeled_image){
  int num_rows = labeled_image.size();
  int num_cols = labeled_image[0].size();
  //cout << "num_rows " << num_rows << " num_cols " << num_cols << endl;
  map<int, int> aggregates_ids;

  int id_given = 0;

  for(int i = 0; i < num_rows; i++){
    for(int j = 0; j < num_cols; j++){
      int pixel = labeled_image[i][j];
      //cout << pixel << endl;
      if(aggregates_ids.count(pixel) > 0) {
        int id = aggregates_ids[pixel];
        aggregates_and_points[id].push_back(make_pair(i,j));
      }
      else if(pixel >= 0){
        aggregates_ids[pixel] = id_given;
        vector<pair<int,int> > first_element;
        first_element.push_back(make_pair(i,j));
        aggregates_and_points.push_back(first_element);
        id_given++;
      }
    }
    //cout << aggregates_and_points.size() << endl;
  }
}

void FORRHallways::ConvertPairToCartesianPoint(vector<vector<CartesianPoint> > &trails, const vector<vector< pair<int,int> > > &input) {
  for(int i = 0; i < input.size(); i++){
    vector<CartesianPoint> trail_coordinates;
    for(int j = 0; j < input[i].size(); j++) {
      CartesianPoint new_point0 = CartesianPoint(input[i][j].first,input[i][j].second);
      trail_coordinates.push_back(new_point0);
      /*CartesianPoint new_point1 = CartesianPoint(input[i][j].first+1.0,input[i][j].second+1.0);
      trail_coordinates.push_back(new_point1);
      CartesianPoint new_point2 = CartesianPoint(input[i][j].first+1.0,input[i][j].second-1.0);
      trail_coordinates.push_back(new_point2);
      CartesianPoint new_point3 = CartesianPoint(input[i][j].first-1.0,input[i][j].second+1.0);
      trail_coordinates.push_back(new_point3);
      CartesianPoint new_point4 = CartesianPoint(input[i][j].first-1.0,input[i][j].second-1.0);
      trail_coordinates.push_back(new_point4);
      CartesianPoint new_point5 = CartesianPoint(input[i][j].first+1.0,input[i][j].second);
      trail_coordinates.push_back(new_point5);
      CartesianPoint new_point6 = CartesianPoint(input[i][j].first-1.0,input[i][j].second);
      trail_coordinates.push_back(new_point6);
      CartesianPoint new_point7 = CartesianPoint(input[i][j].first,input[i][j].second+1.0);
      trail_coordinates.push_back(new_point7);
      CartesianPoint new_point8 = CartesianPoint(input[i][j].first,input[i][j].second-1.0);
      trail_coordinates.push_back(new_point8);*/
    }
    if(trail_coordinates.size() > 2){
      trails.push_back(trail_coordinates);
    }
  }
}

vector<vector<CartesianPoint> > FORRHallways::MergeNearbyHallways(const vector<vector<CartesianPoint> > initial_hallway_groups, const vector<CartesianPoint> &trails, const vector < vector <CartesianPoint> > &laser_history, int hallway_type, double step, int width, int height, double threshold){
  //cout << "Inside MergeNearbyHallways" << endl;
  vector<vector<CartesianPoint> > merged_hallways;
  vector<vector<int> > poses_in_hallways(initial_hallway_groups.size());
  for(int i = 0; i < trails.size(); i++){
    CartesianPoint roundedPoint = CartesianPoint((int)(trails[i].get_x()),(int)(trails[i].get_y()));
    for(int j = 0; j < initial_hallway_groups.size(); j++) {
      std::vector<CartesianPoint>::const_iterator it;
      it = find(initial_hallway_groups[j].begin(), initial_hallway_groups[j].end(), roundedPoint);
      if(it != initial_hallway_groups[j].end())
        poses_in_hallways[j].push_back(i);
    }
  }
  /*for(int i = 0; i < poses_in_hallways.size(); i++){
    cout << poses_in_hallways[i].size() << endl;
  }*/
  vector<Segment> possible_mergers_joins;
  for(int i = 0; i < initial_hallway_groups.size() - 1; i++) {
    for(int j = i + 1; j < initial_hallway_groups.size(); j++) {
      bool first_sees_second, second_sees_first = false;
      Segment temp_segment_fs = Segment(CartesianPoint(0,0),CartesianPoint(0,0));
      Segment temp_segment_sf = Segment(CartesianPoint(0,0),CartesianPoint(0,0));
      for(int k = 0; k < poses_in_hallways[i].size(); k++){
        for(int l = 0; l < initial_hallway_groups[j].size(); l++){
          if(first_sees_second == false){
            if(agent_state->canAccessPoint(laser_history[poses_in_hallways[i][k]], trails[poses_in_hallways[i][k]], initial_hallway_groups[j][l], 20)){
              temp_segment_fs = Segment(trails[poses_in_hallways[i][k]], initial_hallway_groups[j][l]);
              if(temp_segment_fs.GetLeftPoint().get_x() > temp_segment_fs.GetRightPoint().get_x()){
                temp_segment_fs = Segment(initial_hallway_groups[j][l], trails[poses_in_hallways[i][k]]);
              }
              double angle = temp_segment_fs.GetAngle() * 180/M_PI;
              if(hallway_type == 0 and ((angle <= step and angle >= 0) or (angle <= 180 and angle >= 180-step))){
                first_sees_second = true;
              }
              else if(hallway_type == 1 and angle <= 45+step and angle >= 45-step){
                first_sees_second = true;
              }
              else if(hallway_type == 2 and angle <= 90+step and angle >= 90-step){
                first_sees_second = true;
              }
              else if(hallway_type == 3 and angle <= 135+step and angle >= 135-step){
                first_sees_second = true;
              }
            }
          }
        }
      }
      for(int k = 0; k < poses_in_hallways[j].size(); k++){
        for(int l = 0; l < initial_hallway_groups[i].size(); l++){
          if(second_sees_first == false){
            if(agent_state->canAccessPoint(laser_history[poses_in_hallways[j][k]], trails[poses_in_hallways[j][k]], initial_hallway_groups[i][l], 20)){
              temp_segment_sf = Segment(trails[poses_in_hallways[j][k]], initial_hallway_groups[i][l]);
              if(temp_segment_sf.GetLeftPoint().get_x() > temp_segment_sf.GetRightPoint().get_x()){
                temp_segment_sf = Segment(initial_hallway_groups[i][l], trails[poses_in_hallways[j][k]]);
              }
              double angle = temp_segment_sf.GetAngle() * 180/M_PI;
              if(hallway_type == 0 and ((angle <= step and angle >= 0) or (angle <= 180 and angle >= 180-step))){
                second_sees_first = true;
              }
              else if(hallway_type == 1 and angle <= 45+step and angle >= 45-step){
                second_sees_first = true;
              }
              else if(hallway_type == 2 and angle <= 90+step and angle >= 90-step){
                second_sees_first = true;
              }
              else if(hallway_type == 3 and angle <= 135+step and angle >= 135-step){
                second_sees_first = true;
              }
            }
          }
        }
      }
      if(first_sees_second == true and second_sees_first == true){
        //cout << "Both see each other : " << i << " " << j << endl;
        possible_mergers_joins.push_back(temp_segment_fs);
        possible_mergers_joins.push_back(temp_segment_sf);
      }
    }
  }
  if(possible_mergers_joins.size()>0){
    vector<vector<double> > heat_map(width,vector<double>(height, 0));
    for(int i = 0; i < initial_hallway_groups.size(); i++){
      for(int j = 0; j < initial_hallway_groups[i].size(); j++){
        heat_map[round(initial_hallway_groups[i][j].get_x())][round(initial_hallway_groups[i][j].get_y())] = 1;
      }
    }
    for(int i = 0; i < possible_mergers_joins.size(); i++){
      CartesianPoint left = possible_mergers_joins[i].GetLeftPoint();
      CartesianPoint right = possible_mergers_joins[i].GetRightPoint();
      double step = abs(left.get_x() - right.get_x()) + abs(left.get_y() - right.get_y());
      for(double j = 0; j <= 1; j += 1/step) {
        int xcoord = round(j*(right.get_x() - left.get_x()) + left.get_x());
        int ycoord = round(j*(right.get_y() - left.get_y()) + left.get_y());
        heat_map[xcoord][ycoord] = 1;
      }
    }
    
    vector<vector<double> > smoothed_heat_map(width,vector<double>(height, 0));
    SmoothMap(smoothed_heat_map, heat_map, threshold);

    vector<vector<int> > binarized_heat_map(width,vector<int>(height, 0));
    BinarizeImage(binarized_heat_map, smoothed_heat_map, threshold);

    vector<vector<int> > labeled_image(width,vector<int>(height, 0));
    LabelImage(binarized_heat_map,labeled_image);

    vector<vector< pair<int,int> > > points_in_aggregates;
    ListGroups(points_in_aggregates, labeled_image);

    ConvertPairToCartesianPoint(merged_hallways, points_in_aggregates);
    return merged_hallways;
  }
  else{
    return initial_hallway_groups;
  }
}

vector<vector<CartesianPoint> > FORRHallways::FillHallways(const vector<vector<CartesianPoint> > initial_hallway_groups, const vector<CartesianPoint> &trails, const vector < vector <CartesianPoint> > &laser_history, int hallway_type, double step, int width, int height, double threshold){
  //cout << "Inside FillHallways" << endl;
  vector<vector<CartesianPoint> > filled_hallways;
  vector<vector<int> > poses_in_hallways(initial_hallway_groups.size());
  for(int i = 0; i < trails.size(); i++){
    CartesianPoint roundedPoint = CartesianPoint((int)(trails[i].get_x()),(int)(trails[i].get_y()));
    for(int j = 0; j < initial_hallway_groups.size(); j++) {
      std::vector<CartesianPoint>::const_iterator it;
      it = find(initial_hallway_groups[j].begin(), initial_hallway_groups[j].end(), roundedPoint);
      if(it != initial_hallway_groups[j].end())
        poses_in_hallways[j].push_back(i);
    }
  }
  /*for(int i = 0; i < poses_in_hallways.size(); i++){
    cout << poses_in_hallways[i].size() << endl;
  }*/
  vector<Segment> possible_fills;
  for(int i = 0; i < initial_hallway_groups.size(); i++) {
    Segment temp_segment = Segment(CartesianPoint(0,0),CartesianPoint(0,0));
    for(int k = 0; k < poses_in_hallways[i].size(); k++){
      for(int j = 0; j < initial_hallway_groups[i].size(); j++){
        if(agent_state->canAccessPoint(laser_history[poses_in_hallways[i][k]], trails[poses_in_hallways[i][k]], initial_hallway_groups[i][j], 20)){
          temp_segment = Segment(trails[poses_in_hallways[i][k]], initial_hallway_groups[i][j]);
          if(temp_segment.GetLeftPoint().get_x() > temp_segment.GetRightPoint().get_x()){
            temp_segment = Segment(initial_hallway_groups[i][j], trails[poses_in_hallways[i][k]]);
          }
          possible_fills.push_back(temp_segment);
        }
      }
    }
  }
  if(possible_fills.size()>0){
    //cout << "Number of possible fills : " << possible_fills.size() << endl;
    vector<vector<double> > heat_map(width,vector<double>(height, 0));
    for(int i = 0; i < initial_hallway_groups.size(); i++){
      for(int j = 0; j < initial_hallway_groups[i].size(); j++){
        heat_map[round(initial_hallway_groups[i][j].get_x())][round(initial_hallway_groups[i][j].get_y())] = 1;
      }
    }
    for(int i = 0; i < possible_fills.size(); i++){
      CartesianPoint left = possible_fills[i].GetLeftPoint();
      CartesianPoint right = possible_fills[i].GetRightPoint();
      double step = abs(left.get_x() - right.get_x()) + abs(left.get_y() - right.get_y());
      for(double j = 0; j <= 1; j += 1/step) {
        int xcoord = round(j*(right.get_x() - left.get_x()) + left.get_x());
        int ycoord = round(j*(right.get_y() - left.get_y()) + left.get_y());
        heat_map[xcoord][ycoord] = 1;
      }
    }
    
    vector<vector<double> > smoothed_heat_map(width,vector<double>(height, 0));
    SmoothMap(smoothed_heat_map, heat_map, threshold);

    vector<vector<int> > binarized_heat_map(width,vector<int>(height, 0));
    BinarizeImage(binarized_heat_map, smoothed_heat_map, threshold);

    vector<vector<int> > labeled_image(width,vector<int>(height, 0));
    LabelImage(binarized_heat_map,labeled_image);

    vector<vector< pair<int,int> > > points_in_aggregates;
    ListGroups(points_in_aggregates, labeled_image);

    ConvertPairToCartesianPoint(filled_hallways, points_in_aggregates);
    return filled_hallways;
  }
  else{
    return initial_hallway_groups;
  }
}