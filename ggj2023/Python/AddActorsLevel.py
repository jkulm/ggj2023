# WARNING: You must enable the "Python Foundation Packages" plugin for this script to work.

import numpy as np
import unreal

# Set script parameters
spawn_class_path = "/Game/Characters/Flyers/BP_Bird.BP_Bird"
num_spawns = 70
min_dist_between_spawns = 2000
spawn_box_x = 15000
spawn_box_y = 15000
spawn_box_z = 4800

# Function from stack overflow, adapted to generate round numbers
def get_distribution(num_points, min_dist, box_dimensions, maxiter=1e4, allow_wall=True):
    """Get random points in a box with given dimensions and minimum separation.
    
    Parameters:
      
    - num_points: number of points
    - min_dist: minimum distance
    - box_dimensions: dimensions of box, shape (3,) array 
    - maxiter: maximum number of iterations.
    - allow_wall: whether to allow points on wall; 
       (if Fabox_dimensionse: points need to keep distance min_dist/2 from the walbox_dimensions.)
        
    Return:
        
    - ps: array (num_points, 3) of point positions, 
      with 0 <= ps[:, i] < box_dimensions[i]
    - n_iter: number of iterations
    - dratio: average nearest-neighbor distance, divided by min_dist.
    
    Note: with a fill density (sphere volume divided by box volume) above about
    0.53, it takes very long. (Random close-packed spheres have a fill density
    of 0.64).
    
    Author: Han-Kwang Nienhuys (2020)
    Copying: BSD, GPL, LGPL, CC-BY, CC-BY-SA
    See Stackoverflow: https://stackoverflow.com/a/62895898/6228891 
    """
    box_dimensions = np.array(box_dimensions).reshape(3)
    if not allow_wall:
        box_dimensions -= min_dist
    
    # filling factor; 0.64 is for random close-packed spheres
    # This is an estimate because close packing is complicated near the walbox_dimensions.
    # It doesn't work well for small L/min_dist ratios.
    sphere_vol = np.pi/6*min_dist**3
    box_vol = np.prod(box_dimensions + 0.5*min_dist)
    fill_dens = num_points*sphere_vol/box_vol
    if fill_dens > 0.64:
        msg = f'Too many to fit in the volume, density {fill_dens:.3g}>0.64'
        raise ValueError(msg)
    
    # initial try   
    ps = np.random.uniform(size=(num_points, 3)) * box_dimensions
    
    # distance-squared matrix (diagonal is self-distance, don't count)
    dsq = ((ps - ps.reshape(num_points, 1, 3))**2).sum(axis=2)
    dsq[np.arange(num_points), np.arange(num_points)] = np.infty

    for iter_no in range(int(maxiter)):
        # find points that have too close neighbors
        close_counts = np.sum(dsq < min_dist**2, axis=1)  # shape (num_points,)
        n_close = np.count_nonzero(close_counts)
        if n_close == 0:
            break
        
        # Move the one with the largest number of too-close neighbors
        imv = np.argmax(close_counts)
        
        # new positions
        newp = np.random.uniform(size=3)*box_dimensions
        ps[imv]= newp
        
        # update distance matrix
        new_dsq_row = ((ps - newp.reshape(1, 3))**2).sum(axis=-1)
        dsq[imv, :] = dsq[:, imv] = new_dsq_row
        dsq[imv, imv] = np.inf
    else:
        raise RuntimeError(f'Failed after {iter_no+1} iterations.')

    if not allow_wall:
        ps += min_dist/2
    
    dratio = (np.sqrt(dsq.min(axis=1))/min_dist).mean()

    # round each point but keep as float
    ps = np.round(ps, 0).astype(float)

    return ps, iter_no+1, dratio

# Generate spawn points distribution
spawn_points = get_distribution(num_spawns, min_dist_between_spawns, (spawn_box_x, spawn_box_y, spawn_box_z))[0]

# Get a class for spawning
spawn_class = unreal.EditorAssetLibrary.load_blueprint_class(spawn_class_path)

# Get the editor actor subsystem
actor_subsys = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

for spawn_point in spawn_points:
    centered_x = spawn_point[0] - (spawn_box_x / 2)
    centered_y = spawn_point[1] - (spawn_box_x / 2)
    centered_vector = unreal.Vector(centered_x, centered_y, spawn_point[2])
    actor_subsys.spawn_actor_from_class(spawn_class, centered_vector)
